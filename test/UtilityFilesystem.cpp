#include "UtilityFilesystem.h"
#include "Utility/FileSystem/DumpFile.h"
#include "Utility/FileSystem/Filesystem.h"
#include "ulog/ulog.h"
#include <filesystem>
#include <fstream>
#include <string>

void UtilityFilesystem::SetUpTestSuite() {
  ulog::Logger::setFileLogger("tester.log");
}

// ============================================================================
// DumpFile tests
// ============================================================================

TEST_F(UtilityFilesystem, DumpFile_Write) {
  const std::string path = "test_dump.txt";
  const std::string content = "Hello, World!";
  EXPECT_TRUE(utility::filesystem::DumpFile(path, content));

  std::ifstream file(path);
  ASSERT_TRUE(file.is_open());
  std::string read_content;
  std::getline(file, read_content);
  EXPECT_EQ(read_content, content);
  file.close();
  std::filesystem::remove(path);
}

TEST_F(UtilityFilesystem, DumpFile_Append) {
  const std::string path = "test_append.txt";
  EXPECT_TRUE(utility::filesystem::DumpFile(path, "first\n"));
  EXPECT_TRUE(utility::filesystem::DumpFile(path, "second\n", std::ios_base::app));

  std::ifstream file(path);
  ASSERT_TRUE(file.is_open());
  std::string line1, line2;
  std::getline(file, line1);
  std::getline(file, line2);
  EXPECT_EQ(line1, "first");
  EXPECT_EQ(line2, "second");
  file.close();
  std::filesystem::remove(path);
}

TEST_F(UtilityFilesystem, DumpFile_Overwrite) {
  const std::string path = "test_overwrite.txt";
  EXPECT_TRUE(utility::filesystem::DumpFile(path, "original\n"));
  EXPECT_TRUE(utility::filesystem::DumpFile(path, "replaced\n"));

  std::ifstream file(path);
  ASSERT_TRUE(file.is_open());
  std::string line;
  std::getline(file, line);
  EXPECT_EQ(line, "replaced");
  file.close();
  std::filesystem::remove(path);
}

TEST_F(UtilityFilesystem, DumpFile_EmptyContent) {
  const std::string path = "test_empty.txt";
  EXPECT_TRUE(utility::filesystem::DumpFile(path, ""));

  std::ifstream file(path);
  ASSERT_TRUE(file.is_open());
  std::string content;
  std::getline(file, content);
  EXPECT_TRUE(content.empty());
  file.close();
  std::filesystem::remove(path);
}

// ============================================================================
// createDirectory tests
// ============================================================================

TEST_F(UtilityFilesystem, CreateDirectory) {
  const std::filesystem::path dir = "test_create_dir";
  // Ensure clean state
  if (std::filesystem::exists(dir))
    std::filesystem::remove_all(dir);

  utility::filesystem::createDirectory(dir);
  EXPECT_TRUE(std::filesystem::exists(dir));
  EXPECT_TRUE(std::filesystem::is_directory(dir));
  std::filesystem::remove_all(dir);
}

TEST_F(UtilityFilesystem, CreateDirectory_AlreadyExists) {
  const std::filesystem::path dir = "test_existing_dir";
  if (!std::filesystem::exists(dir))
    std::filesystem::create_directory(dir);

  // Should not throw or crash
  EXPECT_NO_THROW(utility::filesystem::createDirectory(dir));
  EXPECT_TRUE(std::filesystem::exists(dir));
  std::filesystem::remove_all(dir);
}

// ============================================================================
// remove / removeAll tests
// ============================================================================

TEST_F(UtilityFilesystem, Remove_ExistingFile) {
  const std::string path = "test_remove_file.txt";
  utility::filesystem::DumpFile(path, "content");
  ASSERT_TRUE(std::filesystem::exists(path));

  EXPECT_TRUE(utility::filesystem::remove(path));
  EXPECT_FALSE(std::filesystem::exists(path));
}

TEST_F(UtilityFilesystem, Remove_NonExisting) {
  EXPECT_FALSE(utility::filesystem::remove("nonexistent_file_xyz.txt"));
}

TEST_F(UtilityFilesystem, Remove_Batch) {
  const std::string path1 = "test_remove_batch_1.txt";
  const std::string path2 = "test_remove_batch_2.txt";
  utility::filesystem::DumpFile(path1, "c");
  utility::filesystem::DumpFile(path2, "c");

  EXPECT_TRUE(utility::filesystem::remove({path1, path2}));
  EXPECT_FALSE(std::filesystem::exists(path1));
  EXPECT_FALSE(std::filesystem::exists(path2));
}

TEST_F(UtilityFilesystem, RemoveAll_Directory) {
  const std::filesystem::path dir = "test_rm_all_dir";
  if (!std::filesystem::exists(dir))
    std::filesystem::create_directory(dir);
  utility::filesystem::DumpFile(
      (dir / "file.txt").string(), "content");

  EXPECT_TRUE(utility::filesystem::removeAll(dir));
  EXPECT_FALSE(std::filesystem::exists(dir));
}

// ============================================================================
// CentralizedDump tests
// ============================================================================

TEST_F(UtilityFilesystem, CentralizedDump) {
  const std::string folder = "test_cdump";
  const std::string filename = "test.txt";
  const std::string content = "centralized content";

  // Clean up first
  if (std::filesystem::exists(folder))
    std::filesystem::remove_all(folder);

  EXPECT_TRUE(
      utility::filesystem::CentralizedDump(folder, filename, content));

  auto path =
      std::filesystem::current_path().append(folder).append(filename);
  EXPECT_TRUE(std::filesystem::exists(path));

  // Read and verify
  std::ifstream file(path);
  std::string read_content;
  std::getline(file, read_content);
  EXPECT_EQ(read_content, content);
  file.close();

  std::filesystem::remove_all(folder);
}
