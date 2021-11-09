#include <opencv2/opencv.hpp>

auto shiftDFT(cv::Mat const & img) {
    // crop the spectrum, if it has an odd number of rows or columns
    cv::Mat t = img( cv::Rect(0, 0, img.cols & -2, img.rows & -2) );

    // rearrange the quadrants of Fourier image so that the origin is at the image center
    int cx = t.cols/2;
    int cy = t.rows/2;

    auto q0 = cv::Rect(0 ,  0, cx, cy);  // Top-Left
    auto q1 = cv::Rect(cx,  0, cx, cy);  // Top-Right
    auto q2 = cv::Rect(0 , cy, cx, cy);  // Bottom-Left
    auto q3 = cv::Rect(cx, cy, cx, cy);  // Bottom-Right

    cv::Mat shifted(t.size(), t.type());

    t(q0).copyTo(shifted(q3));
    t(q3).copyTo(shifted(q0));
    t(q1).copyTo(shifted(q2));
    t(q2).copyTo(shifted(q1));

    return shifted;
}

auto getDFT(cv::Mat const & img) {
    //expand input image to optimal size
    cv::Size dftSize;
    dftSize.width = cv::getOptimalDFTSize( img.cols );
    dftSize.height = cv::getOptimalDFTSize( img.rows );

    // on the border add zero values
    cv::Mat padded_img;
    cv::copyMakeBorder(img, padded_img,
        0, dftSize.height - img.rows, 0, dftSize.width - img.cols,
        cv::BorderTypes::BORDER_CONSTANT, cv::Scalar::all(0)
    );

    cv::Mat dft_img; // output: complex image

    cv::Mat float_img;
    padded_img.convertTo(float_img, CV_32F, 1.0 / 255.0);

    cv::dft(float_img, dft_img, cv::DFT_COMPLEX_OUTPUT);

    return dft_img;
}

// Calculate Magnitude and Phase from Complex Image
std::tuple<cv::Mat, cv::Mat> getMagPhFromComplexImage(const cv::Mat & complex_image) {
    cv::Mat planes[2];
    cv::split(complex_image, planes);

    cv::Mat magnitude(planes[0].size(), planes[0].type());
    cv::Mat phase(planes[0].size(), planes[0].type());

    cv::cartToPolar(planes[0], planes[1], magnitude, phase);
    
    return { magnitude, phase };
}

// Calculate Complex Image from Magnitude and Phase
auto getComplexImageFromMagPh(const cv::Mat & mag, const cv::Mat & ph) {
    cv::Mat planes[2]; 
    
    // planes[0]: real part;
    // planes[1]: imaginary part;
    cv::polarToCart(mag, ph, planes[0], planes[1]);

    cv::Mat complex_img;
    cv::merge(planes, 2, complex_img);
    return complex_img;
}

auto logNormalizeForShow(cv::Mat const & mag) {
    cv::Mat show;
    mag.copyTo(show);
    //show = mag + cv::Scalar::all(1);
    cv::log(show, show);
    cv::normalize(show, show, 0, 1, cv::NormTypes::NORM_MINMAX);
    return show;
}

auto makeBinImageFromText(std::string text) {
    cv::Mat bin_image(cv::Size(19 * text.length(), 32), CV_32F, cv::Scalar::all(0));
    cv::putText(bin_image, text, {0, 28}, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar::all(1), 2, cv::LineTypes::LINE_AA);
    return bin_image;
}

// auto blend_multiply

int main(int argc, char * argv[]) {
    _putenv("QT_AUTO_SCREEN_SCALE_FACTOR=1");

    auto keys = 
        "{help h ? usage |       | Print this message }"
        "{write          |       | Working mode: WRITE; To embed some text in image's spectrum magnitude }"
        "{read           |       | Working mode: READ; To view a image's spectrum magnitude }"
        "{visual         |       | Use imshow to visualize process and result }"
        "{text           |abcdef | Text to be written, default to 'abcdef' }"
        "{@in            |<none> | Input image's path }"
        "{@out           |out.png| Result image's path, default to 'out.png' }";
    
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Blind Watermark v1.0.0");

    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    enum class Mode { WRITE, READ, ERROR };

    Mode mode = Mode::ERROR;

    auto input_image_path = parser.get<std::string>("@in");
    auto output_image_path = parser.get<std::string>("@out");
    auto text = parser.get<std::string>("text");

    auto visual = parser.has("visual");     // Use imshow to visualize result

    auto to_write = parser.has("write");
    auto to_read = parser.has("read");

    if (to_write) {
        mode = Mode::WRITE;
    } else if (to_read) {
        mode = Mode::READ;
    }

    if ( (!to_write) && (!to_read) ) {
        mode = Mode::ERROR;
        std::cerr << "ERROR: Not assigning working mode.\n";
    }

    if (to_write && to_read) {
        mode = Mode::ERROR;
        std::cerr << "ERROR: Only one command should be given each time.\n";
    }

    if (!parser.check() || mode == Mode::ERROR) {
        parser.printMessage();
        parser.printErrors();
        return 0;
    }
    
    cv::Mat img = cv::imread(input_image_path, cv::IMREAD_UNCHANGED);
    cv::Mat out;

    if (img.empty()) {
        std::cout << "Unable to open '" << input_image_path << "'." << std::endl;
        return 1;
    }

    std::cout << input_image_path << ": " << img.size() << " Channels: " << img.channels() << "\n";
    std::cout << "Image loaded successfully." << std::endl;

    if (img.type() == CV_8UC3) cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

    auto dft_img = getDFT(img); // dft_img is a complex_image (2 channels, real and imagine parts)

    auto shifted_dft_img = shiftDFT(dft_img); // shifted dft_img (complex_img)

    auto [magnitude, phase] = getMagPhFromComplexImage(shifted_dft_img);

    cv::Mat log_mag = logNormalizeForShow(magnitude);

    if (visual) {
        cv::namedWindow("img", 0);
        cv::imshow("img", img);

        // cv::Mat show;
        // cv::normalize(magnitude, show, 0, 1, cv::NormTypes::NORM_MINMAX);
        // std::string mag_title_1 = "Spectrum Magnitude (Actual; Normalized)";
        // cv::namedWindow(mag_title_1, 0);
        // cv::imshow(mag_title_1, show);
        
        // Before modified
        std::string mag_title_2 = "Spectrum Magnitude (After Normalize & Log)";
        cv::namedWindow(mag_title_2, 0);
        cv::imshow(mag_title_2, log_mag);
    }

    if (mode == Mode::WRITE) {
        cv::Mat text_image = makeBinImageFromText(text);

        cv::Size wm_size;

        wm_size.width = img.cols / 4;  // 128;
        wm_size.height = wm_size.width / text_image.size().aspectRatio();

        auto wm_area_tl = cv::Rect(0, 0, wm_size.width, wm_size.height);
        auto wm_area_br = cv::Rect(img.cols - wm_size.width, img.rows - wm_size.height, wm_size.width, wm_size.height);
            
        cv::Mat watermark;
        cv::resize(text_image, watermark, wm_size);

        cv::exp(watermark, watermark);

        cv::Mat modified_mag;
        magnitude.copyTo(modified_mag);
        
        watermark.copyTo(modified_mag(wm_area_tl));
        cv::rotate(watermark, watermark, cv::RotateFlags::ROTATE_180);
        watermark.copyTo(modified_mag(wm_area_br));

        cv::rotate(watermark, watermark, cv::RotateFlags::ROTATE_180);

        auto modified_dft_img = shiftDFT(getComplexImageFromMagPh(modified_mag, phase)); // complex image
        
        cv::Mat modified_img;  // real image
        cv::idft(modified_dft_img, modified_img, cv::DFT_REAL_OUTPUT);
        
        cv::normalize(modified_img, modified_img, 0, 1, cv::NormTypes::NORM_MINMAX);
        modified_img.convertTo(out, CV_8UC1, 255);
        
        std::cout << "Input image size: " << img.size() << "\n" << "Result image size: " << out.size();

        if (visual) {
            cv::namedWindow("Watermark", 0);
            cv::imshow("Watermark", logNormalizeForShow(watermark));

            std::string modified_mag_win_title = "Modified Spectrum Magnitude (After Normalize & Log)";
            cv::namedWindow(modified_mag_win_title, 0);
            cv::imshow(modified_mag_win_title, logNormalizeForShow(modified_mag));

            cv::namedWindow("result", 0);
            cv::imshow("result", modified_img);
            cv::waitKey(0);
        }

    } else if (mode == Mode::READ) {
        if (visual) {
            cv::waitKey(0);
        }
        log_mag.convertTo(out, CV_8UC1, 255);
    }
    
    cv::imwrite(output_image_path, out);

    return 0;
}

