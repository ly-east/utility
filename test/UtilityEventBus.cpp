#include "Utility/EventBus/EventBus.h"
#include "gtest/gtest.h"

namespace {

struct TestEvent : utility::eventbus::EventBase {
  int value = 0;
};

class UtilityEventBus : public testing::Test {
public:
  static void SetUpTestSuite() {}

  void TearDown() override {
    // Clean up: unsubscribe any leftover subscriptions
    // EventBus is a singleton, so stale handlers from other tests may persist.
    // We rely on each test cleaning up its own token.
  }
};

} // namespace

TEST_F(UtilityEventBus, SubscribeAndPublish) {
  auto &bus = utility::eventbus::EventBus::Instance();
  bool called = false;
  int received = 0;

  TestEvent ev;
  ev.value = 42;

  auto token = bus.Subscribe<TestEvent>(
      [&called, &received](const utility::eventbus::EventBase &base) {
        called = true;
        received = static_cast<const TestEvent &>(base).value;
      });

  bus.Publish(ev);
  EXPECT_TRUE(called);
  EXPECT_EQ(received, 42);

  bus.Unsubscribe(token);
}

TEST_F(UtilityEventBus, PublishWithoutSubscriber) {
  auto &bus = utility::eventbus::EventBus::Instance();
  TestEvent ev;
  EXPECT_NO_THROW(bus.Publish(ev));
}

TEST_F(UtilityEventBus, UnsubscribeThenPublish) {
  auto &bus = utility::eventbus::EventBus::Instance();
  bool called = false;

  auto token = bus.Subscribe<TestEvent>(
      [&called](const utility::eventbus::EventBase &) { called = true; });

  bus.Unsubscribe(token);
  bus.Publish(TestEvent{});
  EXPECT_FALSE(called);
}

TEST_F(UtilityEventBus, MultipleSubscribers) {
  auto &bus = utility::eventbus::EventBus::Instance();
  int count = 0;

  auto token1 = bus.Subscribe<TestEvent>(
      [&count](const utility::eventbus::EventBase &) { ++count; });
  auto token2 = bus.Subscribe<TestEvent>(
      [&count](const utility::eventbus::EventBase &) { ++count; });

  bus.Publish(TestEvent{});
  EXPECT_EQ(count, 2);

  bus.Unsubscribe(token1);
  bus.Unsubscribe(token2);
}

TEST_F(UtilityEventBus, DifferentEventTypes) {
  auto &bus = utility::eventbus::EventBus::Instance();

  struct OtherEvent : utility::eventbus::EventBase {};
  bool test_called = false;
  bool other_called = false;

  auto token1 = bus.Subscribe<TestEvent>(
      [&test_called](const utility::eventbus::EventBase &) { test_called = true; });
  auto token2 = bus.Subscribe<OtherEvent>(
      [&other_called](const utility::eventbus::EventBase &) { other_called = true; });

  bus.Publish(TestEvent{});
  EXPECT_TRUE(test_called);
  EXPECT_FALSE(other_called);

  bus.Publish(OtherEvent{});
  EXPECT_TRUE(other_called);

  bus.Unsubscribe(token1);
  bus.Unsubscribe(token2);
}
