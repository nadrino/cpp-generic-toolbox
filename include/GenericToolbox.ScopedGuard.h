//
// Created by Nadrino on 15/05/2023.
//

#ifndef GUNDAM_GENERICTOOLBOX_SCOPEDGUARD_H
#define GUNDAM_GENERICTOOLBOX_SCOPEDGUARD_H


#include <functional>


namespace GenericToolbox{
  class ScopedGuard{

  public:
    using Action = std::function<void()>;

    explicit ScopedGuard(const Action& onCreate_, Action onDelete_) : _onDelete_{std::move(onDelete_)} {
      if( onCreate_ ){ onCreate_(); }
    }
    ~ScopedGuard() {
      fireOnDelete();
    }

    void fireOnDelete(){
      if( _onDelete_ ){ _onDelete_(); }
      this->dismiss();
    }
    void dismiss(){ _onDelete_ = []{}; }

    // non-copiable
    ScopedGuard() = default;
    ScopedGuard( ScopedGuard const& ) = delete;
    ScopedGuard( ScopedGuard&& ) = default;
    auto operator=( ScopedGuard&& ) -> ScopedGuard& = default;
    auto operator=( ScopedGuard const& ) -> ScopedGuard& = delete;

  private:
    Action _onDelete_{};

  };
}

#endif //GUNDAM_GENERICTOOLBOX_SCOPEDGUARD_H
