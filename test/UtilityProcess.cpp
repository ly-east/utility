#include "UtilityProcess.h"
#include "Utility/Process/Launch.h"
#include "Utility/Process/Terminate.h"
#include "Utility/String/Encoding.h"
#include "Utility/String/Type.h"
#include "ulog/ulog.h"
#include <string>
#include <vector>

#if defined(__linux__)
#include <chrono>
#include <future>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#endif

void UtilityProcess::SetUpTestSuite() {
  ulog::Logger::setFileLogger("tester.log");
}

TEST_F(UtilityProcess, locateProgram) {
  using utility::string::Str;

  const std::vector<Str> program{
#if defined(_WIN32)
      L"ccleaner", L"git", L"python"
#elif defined(__linux__)
      "sh", "git"
#endif
  };

  for (const auto &p : program) {
    const Str where{utility::process::locateProgram(p)};
    EXPECT_FALSE(where.empty());
#if defined(_WIN32)
    ulg.debug("{}: {}", utility::string::unicodeToUTF8(p),
              utility::string::unicodeToUTF8(where));
#elif defined(__linux__)
    ulg.debug("{}: {}", p, where);
#endif
  }
}

TEST_F(UtilityProcess, locateProgramNotFound) {
  EXPECT_TRUE(
      utility::process::locateProgram(TXT("nonexistent_program_xyz")).empty());
}

TEST_F(UtilityProcess, getExecutableFilePath) {
#if defined(_WIN32)
  EXPECT_FALSE(utility::process::getExecutableFilePath(L"cmd").empty());
#elif defined(__linux__)
  EXPECT_FALSE(utility::process::getExecutableFilePath("sh").empty());
#endif
  EXPECT_TRUE(
      utility::process::getExecutableFilePath(TXT("nonexistent_program_xyz"))
          .empty());
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

// ============================================================================
// launchHiddenProgram tests (Linux: command line tokenization, redirection,
// promise signaling and exit codes)
// ============================================================================

TEST_F(UtilityProcess, launchHiddenProgramQuotedArgs) {
#if defined(__linux__)
  std::string output;
  // double quotes group arguments containing whitespace
  utility::string::Str arg{"printf \"%s %s\\n\" \"hello world\" again"};

  const int exit_code = utility::process::launchHiddenProgram(
      "printf", arg.data(), [&output](std::string &&str) { output += str; });

  EXPECT_EQ(exit_code, 0);
  EXPECT_EQ(output, "hello world again");
#else
  GTEST_SKIP() << "Linux-specific command line tokenization test";
#endif
}

TEST_F(UtilityProcess, launchHiddenProgramNullArg) {
#if defined(__linux__)
  // nullptr arg: argv[0] falls back to the program path
  EXPECT_EQ(utility::process::launchHiddenProgram("true", nullptr,
                                                  [](std::string &&) {}),
            0);
#else
  GTEST_SKIP() << "Linux-specific test";
#endif
}

TEST_F(UtilityProcess, launchHiddenProgramExitCode) {
#if defined(__linux__)
  EXPECT_EQ(utility::process::launchHiddenProgram("false", nullptr,
                                                  [](std::string &&) {}),
            1);
#else
  GTEST_SKIP() << "Linux-specific test";
#endif
}

TEST_F(UtilityProcess, launchHiddenProgramRedirectsStderr) {
#if defined(__linux__)
  std::string output;
  utility::string::Str arg{"sh -c \"echo out; echo err 1>&2\""};

  const int exit_code = utility::process::launchHiddenProgram(
      "sh", arg.data(), [&output](std::string &&str) { output += str; });

  EXPECT_EQ(exit_code, 0);
  EXPECT_NE(output.find("out"), std::string::npos);
  EXPECT_NE(output.find("err"), std::string::npos);
#else
  GTEST_SKIP() << "Linux-specific test";
#endif
}

TEST_F(UtilityProcess, launchHiddenProgramPromise) {
#if defined(__linux__)
  std::promise<bool> p;
  auto result = p.get_future();

  utility::process::launchHiddenProgram("true", nullptr,
                                        [](std::string &&) {}, &p);
  EXPECT_TRUE(result.get());
#else
  GTEST_SKIP() << "Linux-specific test";
#endif
}

TEST_F(UtilityProcess, launchHiddenProgramExecFailure) {
#if defined(__linux__)
  std::string output;
  utility::string::Str arg{"nonexistent_program_xyz --flag"};

  // exec failure is reported through the pipe and the child exits with 127
  const int exit_code = utility::process::launchHiddenProgram(
      "nonexistent_program_xyz", arg.data(),
      [&output](std::string &&str) { output += str; });

  EXPECT_EQ(exit_code, 127);
  EXPECT_NE(output.find("execvp"), std::string::npos);
#else
  GTEST_SKIP() << "Linux-specific test";
#endif
}

// ============================================================================
// terminateProcess tests
// ============================================================================

TEST_F(UtilityProcess, terminateProcess) {
#if defined(__linux__)
  // fork a child and rename it through PR_SET_NAME so the test needs no
  // external long-running binary and can never match another process
  const pid_t pid = fork();
  ASSERT_NE(pid, -1);

  if (pid == 0) {
    prctl(PR_SET_NAME, (unsigned long)"ut_sleeper", 0, 0, 0);
    for (;;)
      pause();
    _exit(0);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(utility::process::terminateProcess("ut_sleeper"));

  int status = 0;
  ASSERT_EQ(waitpid(pid, &status, 0), pid);
  EXPECT_TRUE(WIFSIGNALED(status));
  EXPECT_EQ(WTERMSIG(status), SIGKILL);

  // the process is gone now
  EXPECT_FALSE(utility::process::terminateProcess("ut_sleeper"));
#else
  GTEST_SKIP() << "Linux-specific test";
#endif
}
