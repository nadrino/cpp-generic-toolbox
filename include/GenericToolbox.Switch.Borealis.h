//
// Created by Adrien Blanchet on 07/05/2023.
//

#ifndef SIMPLEMODMANAGER_GENERICTOOLBOX_SWITCH_BOREALIS_H
#define SIMPLEMODMANAGER_GENERICTOOLBOX_SWITCH_BOREALIS_H

#ifdef __SWITCH__

#include "borealis.hpp"

#include "string"
#include "functional"


namespace GenericToolbox::Switch::Borealis{

  const NVGcolor redNvgColor{nvgRGB(0xff, 0x64, 0x64)};
  const NVGcolor greenNvgColor{nvgRGB(0x00, 0xff, 0xc8)};
  const NVGcolor blueNvgColor{nvgRGB(0x00, 0xc8, 0xff)};
  const NVGcolor grayNvgColor{nvgRGB(80, 80, 80)};
  const NVGcolor orangeNvgColor{nvgRGB(208, 168, 50)};// (unsigned char) (245*0.85), (unsigned char) (198*0.85), (unsigned char) (59*0.85)


  class ProgressBarMonitorView : public brls::View {

  public:
    inline ProgressBarMonitorView() = default;
    inline ~ProgressBarMonitorView() override;

    // setters
    inline void setHeaderTitle(const std::string &header);
    inline void setProgressColor(const NVGcolor &progressColor_);
    inline void setExecOnDelete(const std::function<void()> &execOnDelete_);

    inline void setTitlePtr(const std::string *titlePtr);
    inline void setSubTitlePtr(const std::string *subTitlePtr);
    inline void setProgressFractionPtr(double *progressFractionPtr);
    inline void setSubProgressFractionPtr(double *subProgressFractionPtr);

    // misc
    inline void resetMonitorAddresses();

    // overrides
    inline void draw(
        NVGcontext* vg, int x, int y, unsigned width, unsigned height,
        brls::Style* style, brls::FrameContext* ctx) override;

  protected:
    inline void drawProgressBar(NVGcontext *vg, int x, unsigned int width, unsigned int yPosition_, double fraction_);

  private:
    // user parameters
    std::string _header_{};
    NVGcolor _progressColor_{GenericToolbox::Switch::Borealis::greenNvgColor};
    std::function<void()> _execOnDelete_{};

    // monitoring
    const double* _subProgressFractionPtr_{nullptr};
    const double* _progressFractionPtr_{nullptr};
    const std::string* _titlePtr_{nullptr};
    const std::string* _subTitlePtr_{nullptr};
  };

  class PopupLoadingBox {

  public:
    inline PopupLoadingBox() = default;

    [[nodiscard]] inline ProgressBarMonitorView *getMonitorView() const;

    inline void pushView();
    inline void popView();


  private:
    // memory handled by brls
    brls::Dialog* _loadingBox_{nullptr};
    ProgressBarMonitorView* _monitorView_{nullptr};
  };




}

#include "implementation/GenericToolbox.Switch.Borealis.impl.h"

#endif

#endif //SIMPLEMODMANAGER_GENERICTOOLBOX_SWITCH_BOREALIS_H
