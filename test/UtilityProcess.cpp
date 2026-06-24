#include "UtilityProcess.h"
#include "Utility/Process/Launch.h"
#include "Utility/Process/Terminate.h"
#include "Utility/String/Encoding.h"
#include "Utility/String/Type.h"
#include "ulog/ulog.h"
#include <string>
#include <vector>

void UtilityProcess::SetUpTestSuite() {
  ulog::Logger::setFileLogger("tester.log");
}

TEST_F(UtilityProcess, locateProgram) {
  using utility::string::Str;

  const std::vector<Str> program{
#if defined(_WIN32)
      L"ccleaner", L"git", L"python"
#elif defined(__linux__)
      "ccleaner", "git", "python"
#endif
  };

  Str where;

  for (const auto &p : program) {
    where = utility::process::locateProgram(p);
    EXPECT_EQ(where.empty(), false);
    ulg.debug("{}: {}", utility::string::unicodeToUTF8(p),
              utility::string::unicodeToUTF8(where));
  }

  utility::process::launchHiddenProgram(where, nullptr, [](std::string &&) {});
}

TEST_F(UtilityProcess, killNonExistedProcess) {
  utility::string::Str str{
#if defined(_WIN32)
      L"RavMonD.exe"
#elif defined(__linux__)
      "calc.exe"
#endif
  };

  EXPECT_FALSE(utility::process::terminateProcess(str));
}
