
using namespace cv;

int
main(int argc, char* argv[]) {
    log_error(PROJECT_INFO_STRING_FULL);
    ERROR_IF(42 == 42, "I guess it's meant to break");
    return 0;
}
