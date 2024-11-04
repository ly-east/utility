#include "Utility/Crash/CrashPad.h"
#include "client/crash_report_database.h"
#include "client/crashpad_client.h"
#include "client/crashpad_info.h"
#include "client/settings.h"
#include "ulog/ulog.h"
#include <cstdlib>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace {
static std::unique_ptr<crashpad::CrashReportDatabase> database;
using StringType = base::FilePath::StringType;

static bool startCrashHandler(StringType const &handler_path,
                              StringType const &db_path) {
  using namespace crashpad;

  std::map<std::string, std::string> annotations;
  std::vector<std::string> arguments;

  /*
   * THE FOLLOWING ANNOTATIONS MUST BE SET.
   *
   * Backtrace supports many file formats. Set format to minidump
   * so it knows how to process the incoming dump.
   */
  annotations["format"] = "minidump";

  /*
   * REMOVE THIS FOR ACTUAL BUILD.
   *
   * We disable crashpad rate limiting for this example.
   */
  arguments.push_back("--no-rate-limit");

  base::FilePath db{db_path};
  base::FilePath handler{handler_path};

  database = crashpad::CrashReportDatabase::Initialize(db);

  if (!database || !database->GetSettings())
    return false;

  return CrashpadClient{}.StartHandler(handler, db, db, "", annotations,
                                       arguments, false, true, {});
}

bool setHandler(bool is_console) {
  std::string handler_name{"handler"};
  if (is_console)
    handler_name.append("_console");

#if defined(_WIN32)
  handler_name.append(".exe");
#endif

  /*
   * ENSURE THIS VALUE IS CORRECT.
   *
   * Crashpad has the ability to support crashes both in-process
   * and out-of-process. The out-of-process handler is
   * significantly more robust than traditional in-process crash
   * handlers. This path may be relative.
   */

  auto current_path = std::filesystem::current_path();
  auto handler_path = std::filesystem::path{current_path} / handler_name;
  if (!std::filesystem::exists(handler_path)) {
    ulg.error("setHandler: {} doesn't exist", handler_path.string());
    return false;
  }

  /*
   * ENSURE THIS VALUE IS CORRECT.
   *
   * This is the directory you will use to store and
   * queue crash data.
   */

  auto db_path = std::filesystem::path{current_path} / "crash";
  if (!std::filesystem::exists(db_path))
    std::filesystem::create_directories(db_path);

#if defined(_WIN32)
  return startCrashHandler(handler_path.wstring(), db_path.wstring());
#else
  return startCrashHandler(handler_path.string(), db_path.string());
#endif
}
} // namespace

namespace utility {
namespace crash {
bool setConsoleHandler() { return setHandler(true); }

bool setWindowHandler() { return setHandler(false); }

void crashTest() { memset((void *)(intptr_t)4, 123, 1); }
} // namespace crash
} // namespace utility
