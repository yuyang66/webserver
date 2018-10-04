#pragma once

namespace yy_webserver {
	template <typename Lock>
	class AutoLock {
	private:
		Lock* lock_;
		AutoLock(const AutoLock & other) {}
		AutoLock&  operator = (const AutoLock & other){}

	public:
		explicit AutoLock(Lock* lock) {
			lock_ = lock;
			lock_->lock();
		}
		~AutoLock() {
			lock_->unlock();
		}
	};
}
