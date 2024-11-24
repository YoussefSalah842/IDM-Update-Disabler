#include "gtest/gtest.h"
#include "RegistryTool.h"

TEST(RegistryUtilsTest, TestBackupRegistryKey) {
    // Test backup functionality
    ASSERT_NO_THROW(BackupRegistryKey(nullptr));
}

TEST(RegistryUtilsTest, TestDisableUpdates) {
    // Test disabling updates
    ASSERT_NO_THROW(DisableUpdates(nullptr));
}
