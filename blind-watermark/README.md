# Blind Watermark

Lab for Image Steganography


## Usage

Currently you can build this from Source.

Build requirements (Prequisities):

- C++ 17
- OpenCV 4
- CMake

### Command Line Options

- For **help** message:
    
    ```console
    $ ./blind-wm help
    ```
    
- To **write text into** image:

    ```console
    $ ./blind-wm --write in.png out.png
    ```

    The result should be written to `out.png`.

- To **read from image** (to view image's spectrum magnitude):

    ```console
    $ ./blind-wm --read in.png out.png
    ```

    The result should be written to `out.png`.

Add `--visual` to use `cv::imshow` to visualize the process and the result.
