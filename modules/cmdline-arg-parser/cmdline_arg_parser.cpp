#include <cassert>
#include <glog/logging.h>
#include "cmdline_arg_parser.h"

/**
 * \note DEFINE flags declared in .h file here. Refer to https://gflags.github.io/gflags/.
 * \warning Flags will be initialized before the program entering the main function!
 *   This means any error occurring here will not be caught unless you're using debugger.
 *   (Thus, do not use this variable in any other place and you should not modify it.)
 */
DEFINE_string(type, "", "controller type");
DEFINE_bool(camera, false, "run with camera");
DEFINE_bool(serial, false, "run with serial communication");
DEFINE_bool(gimbal, false, "run with gimbal control");

DEFINE_int32(mode_chooser, 0, "controller running mode chooser");

// TODO Temporary flag for debug, will be removed in the future.
DEFINE_bool(rune, false, "run with rune, must under infantry controller");
DEFINE_bool(debug_image, false, "in debug mode show image");
DEFINE_bool(debug_trackbar, true, "in debug use trackbar");

void CmdlineArgParser::Parse(int argc, char **argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // Set GLog flags.
    // ================================================
    google::InitGoogleLogging(argv[0]);
    // STDERR settings.
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    FLAGS_log_prefix = true;
    // File settings.
    FLAGS_log_dir = "../log";
    FLAGS_stop_logging_if_full_disk = true;
    FLAGS_max_log_size = 16;

    run_with_camera_ = FLAGS_camera;
    run_with_gimbal_ = FLAGS_gimbal;
    run_with_serial_ = FLAGS_serial;
    controller_type_ = FLAGS_type;

    mode_chooser_ = FLAGS_mode_chooser;
    debug_show_image_ = FLAGS_debug_image;
    debug_use_trackbar_ = FLAGS_debug_trackbar;
    run_mode_rune_ = FLAGS_rune;

    // You must enable gimbal control to establish serial communication.
    assert(!run_with_serial_ || run_with_gimbal_);

    // Rune mode must be run in infantry controller.
    assert(!run_mode_rune_ || controller_type_ == "infantry");

    LOG(INFO) << "Running " << (run_with_camera_ ? "with" : "without") << " camera.";
    LOG(INFO) << "Running " << (run_with_serial_ ? "with" : "without") << " serial communication.";
    LOG(INFO) << "Running " << (run_with_gimbal_ ? "with" : "without") << " gimbal control.";
    LOG(INFO) << "Running " << (run_mode_rune_ ? "with" : "without") << " rune mode.";
    LOG(INFO) << "Controller type: " << controller_type_;
}
