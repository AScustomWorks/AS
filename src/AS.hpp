#include "rack.hpp"
//#include "rack0.hpp"


#pragma once

using namespace rack;

extern Plugin *pluginInstance;

////////////////////
// module widgets
////////////////////

extern Model *modelSineOsc;
extern Model *modelSawOsc;
extern Model *modelADSR;
extern Model *modelVCA;
extern Model *modelQuadVCA;
extern Model *modelTriLFO;
extern Model *modelAtNuVrTr;
extern Model *modelBPMClock;
extern Model *modelSEQ16;
extern Model *modelMixer2ch;
extern Model *modelMixer4ch;
extern Model *modelMixer8ch;
extern Model *modelMonoVUmeter;
extern Model *modelStereoVUmeter;
extern Model *modelMultiple2_5;
extern Model *modelMerge2_5;
extern Model *modelSteps;

extern Model *modelTriggersMKI;
extern Model *modelTriggersMKII;
extern Model *modelTriggersMKIII;
extern Model *modelLaunchGate;
extern Model *modelKillGate;

extern Model *modelFlow;
extern Model *modelSignalDelay;

extern Model *modelBPMCalc;
extern Model *modelBPMCalc2;

extern Model *modelCv2T;
extern Model *modelZeroCV2T;
extern Model *modelReScale;

extern Model *modelDelayPlusFx;
extern Model *modelDelayPlusStereoFx;

extern Model *modelPhaserFx;
extern Model *modelReverbFx;
extern Model *modelReverbStereoFx;

extern Model *modelSuperDriveFx;

extern Model *modelTremoloFx;
extern Model *modelTremoloStereoFx;

extern Model *modelWaveShaper;

extern Model *modelBlankPanel4;
extern Model *modelBlankPanel6;
extern Model *modelBlankPanel8;
extern Model *modelBlankPanelSpecial;


//////////////////////////////////////
//MODULE COMPONENTS
/////////////////////////////////////

struct as_HexScrew : app::SvgScrew {
	as_HexScrew() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as-hexscrew.svg")));
		sw->wrap();
		box.size = sw->box.size;
	}
};

struct as_Knob : app::SvgKnob {
	as_Knob() {
        minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as-knob.svg")));
	}
};
struct as_KnobBlack : app::SvgKnob {
	as_KnobBlack() {
        minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as-knobBlack.svg")));
	}
};

struct as_KnobBlackSnap : as_KnobBlack {
	as_KnobBlackSnap() {
		snap = true;
	}
};

struct as_KnobBlackSnap4 : as_KnobBlack {
	as_KnobBlackSnap4() {
		minAngle = -0.30 * M_PI;
		maxAngle = 0.30 * M_PI;
		snap = true;
	}
};


struct as_FxKnobWhite : app::SvgKnob {
	as_FxKnobWhite() {
        minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as-FxKnobWhite.svg")));
	}
};
struct as_FxKnobBlack : app::SvgKnob {
	as_FxKnobBlack() {
        minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as-FxKnobBlack.svg")));
	}
};

struct BigLEDBezel : app::SvgSwitch {
        BigLEDBezel() {
				momentary = true;
                addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_bigLEDBezel.svg")));
        }
};

struct as_CKSS : app::SvgSwitch {
	as_CKSS() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_CKSS_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_CKSS_1.svg")));
	}
};

struct as_CKSSH : app::SvgSwitch {
	as_CKSSH() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_CKSSH_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_CKSSH_1.svg")));
	}
};

struct as_CKSSThree : app::SvgSwitch {
	as_CKSSThree() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_CKSSThree_2.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_CKSSThree_1.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_CKSSThree_0.svg")));
	}
};

struct as_MuteBtn : app::SvgSwitch {
	as_MuteBtn() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_mute-off.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_mute-on.svg")));
	}
};

struct as_SoloBtn : app::SvgSwitch {
	as_SoloBtn() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_solo-off.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_solo-on.svg")));
	}
};

template <typename BASE>
struct GiantLight : BASE {
        GiantLight() {
                this->box.size = mm2px(Vec(18.0, 18.0));
        }
};

template <typename BASE>
 struct MeterLight : BASE {
 	MeterLight() {
 		this->box.size = Vec(8, 8);
		 this->bgColor = nvgRGB(0x14, 0x14, 0x14);
 	}
 };

 template <typename BASE>
 struct DisplayLedLight : BASE {
 	DisplayLedLight() {
 		this->box.size = mm2px(Vec(2.0, 2.0));
		 this->bgColor = nvgRGB(0x14, 0x14, 0x14);
 	}
 };
 struct OrangeLight : GrayModuleLightWidget {
	OrangeLight() {
		addBaseColor(SCHEME_ORANGE);
	}
};

template <typename BASE>
struct LedLight : BASE {
	LedLight() {
	  //this->box.size = Vec(20.0, 20.0);
	  this->box.size = mm2px(Vec(6.0, 6.0));
	}
};

/** Reads two adjacent lightIds, so `lightId` and `lightId + 1` must be defined */
struct YellowRedLight : GrayModuleLightWidget {
	YellowRedLight() {
		addBaseColor(SCHEME_YELLOW);
		addBaseColor(SCHEME_RED);
	}
};

struct as_PJ301MPort : app::SvgPort {
	as_PJ301MPort() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as-PJ301M.svg")));
	}
};

struct as_SlidePot : app::SvgSlider {
	as_SlidePot() {
		Vec margin = Vec(4, 4);
		maxHandlePos = Vec(-1.5, -8).plus(margin);
		minHandlePos = Vec(-1.5, 87).plus(margin);
		setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/as-SlidePot.svg")));
		background->wrap();
		background->box.pos = margin;
		box.size = background->box.size.plus(margin.mult(2));
		setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/as-SlidePotHandle.svg")));
		handle->wrap();
	}
};

struct as_FaderPot : app::SvgSlider {
	as_FaderPot() {
		Vec margin = Vec(4, 4);
		maxHandlePos = Vec(-1.5, -8).plus(margin);
		minHandlePos = Vec(-1.5, 57).plus(margin);
		setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/as-FaderPot.svg")));
		background->wrap();
		background->box.pos = margin;
		box.size = background->box.size.plus(margin.mult(2));
		setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/as-SlidePotHandle.svg")));
		handle->wrap();
	}
};
