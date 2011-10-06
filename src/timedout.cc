#include <vector>
#include <utility>
#include <v8.h>
#include <v8-debug.h>
#include <node.h>
#include <unistd.h>
#include <iv/singleton.h>
#include "timedout.h"
namespace {

class Counter : public iv::core::Singleton<Counter> {
 public:
  friend class iv::core::Singleton<Counter>;
  typedef std::vector<std::pair<uint64_t, uint32_t> > Timers;

  class CounterHolder {
   public:
    CounterHolder(uint64_t start, uint32_t time) {
      Counter::Instance()->Start(start, time);
    }

    ~CounterHolder() {
      Counter::Instance()->Finish();
    }
  };

  static void Handler(const v8::Debug::EventDetails& details) {
    Instance()->Process(details);
  }

  void Init(v8::Isolate* isolate) {
    isolate_ = isolate;
  }

  void Process(const v8::Debug::EventDetails& details) {
    if (v8::V8::IsDead()) {
      return;
    }
    const uint64_t now = GetCurrentTime();
    if ((current_ + timedout::kPreemptionMs) < now) {
      for (Timers::const_iterator it = counters_.begin(),
           last = counters_.end(); it != last; ++it) {
        if (now > (it->first + it->second)) {
          v8::V8::TerminateExecution(isolate_);
          return;
        }
      }
      current_ = now;
    }
    if (!counters_.empty()) {
      v8::Debug::DebugBreak();
    }
  }

  static uint64_t GetCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + (tv.tv_usec / 1000);
  }

  static v8::Handle<v8::Value> Core(const v8::Arguments& args) {
    v8::HandleScope scope;
    if (args.Length() == 2) {
      v8::Local<v8::Uint32> time = args[1]->ToUint32();
      const CounterHolder holder(GetCurrentTime(), time->Value());
      v8::Local<v8::Function>::Cast(args[0])->Call(args.This(), 0, NULL);
    }
  }

  void Start(uint64_t start, uint32_t time) {
    const uint64_t id = counters_.size();
    counters_.push_back(std::make_pair(start, time));
    if (id == 0) {
      v8::Debug::DebugBreak();
    }
  }

  void Finish() {
    counters_.pop_back();
    if (counters_.empty()) {
      v8::Debug::CancelDebugBreak();
    }
  }

 private:
  Counter() : current_(GetCurrentTime()), counters_() {
    v8::Debug::SetDebugEventListener2(&Handler);
  }

  ~Counter() { }  // private destructor

  v8::Isolate* isolate_;
  uint64_t current_;
  Timers counters_;
};

}  // namespace anonymous

extern "C"
void init(v8::Handle<v8::Object> target) {
  v8::HandleScope scope;
  v8::Handle<v8::FunctionTemplate> func = v8::FunctionTemplate::New(&Counter::Core);
  target->Set(v8::String::New("timedout"), func->GetFunction());
  Counter::Instance()->Init(v8::Isolate::GetCurrent());
}
