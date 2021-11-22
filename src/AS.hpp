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
extern Model *modelSuperDriveStereoFx;

extern Model *modelTremoloFx;
extern Model *modelTremoloStereoFx;

extern Model *modelWaveShaper;
extern Model *modelWaveShaperStereo;

extern Model *modelBlankPanel4;
extern Model *modelBlankPanel6;
extern Model *modelBlankPanel8;
extern Model *modelBlankPanelSpecial;


//////////////////////////////////////
//MODULE COMPONENTS
/////////////////////////////////////

struct as_HexScrew : app::SvgScrew {
	as_HexScrew() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_hexscrew.svg")));
		//setSvg(Svg::load(asset::plugin(pluginInstance, "res/v2/as_hexscrew.svg")));
		sw->wrap();
		box.size = sw->box.size;
	}
};
//Update For V2 for the 3D looks
struct AsBaseKnob : app::SvgKnob {

	widget::SvgWidget* bg;
	widget::SvgWidget* fg;

	AsBaseKnob() {
		//rotation angles, override in custon knob struct as needed
		minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;
		//The actual rotating graphic is defined on each struct setting with setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as-knob.svg")));
		//background static graphic with highlight and gradients as needed
		bg = new widget::SvgWidget;
		fb->addChildBelow(bg, tw);
		//foreground (knob cap)-> a static graphic with highlights and gradients as needed
		fg = new widget::SvgWidget;
		fb->addChildAbove(fg, tw);
	}
};

struct as_Knob : AsBaseKnob {

	as_Knob() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/as_knob.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/as_knob_bg.svg")));
	}
};
struct as_KnobBlack : AsBaseKnob {
	as_KnobBlack() {
        setSvg(Svg::load(asset::plugin(pluginInstance, "res/as_knobBlack.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/as_knobBlack_bg.svg")));
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


struct as_FxKnobWhite : AsBaseKnob {
	as_FxKnobWhite() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/as_FxKnobWhite.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/as_FxKnobWhite_bg.svg")));
	}
};
struct as_FxKnobBlack : AsBaseKnob {
	as_FxKnobBlack() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/as_FxKnobBlack.svg")));
        bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/as_FxKnobBlack_bg.svg")));
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

struct as_CKSSwhite : app::SvgSwitch {
	as_CKSSwhite() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_CKSS_white_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_CKSS_white_1.svg")));
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
/*updated jumbo leds*/
struct JumboLEDBezel44 : app::SvgSwitch {
        JumboLEDBezel44() {
			momentary = true;
            addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_JumboLEDBezel44_0.svg")));
        }
};

struct JumboLEDBezel60 : app::SvgSwitch {
        JumboLEDBezel60() {
			momentary = true;
            addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_JumboLEDBezel60_0.svg")));
        }
};



template <typename BASE>
struct JumboLedLight60 : BASE {
	JumboLedLight60() {
		this->box.size = Vec(48.0, 48.0);
	}
};

struct JumboLedSwitch60 : app::SvgSwitch {
	JumboLedSwitch60() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_JumboLEDBezel60_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_JumboLEDBezel60_1.svg")));
	}
};

template <typename BASE>
struct JumboLedLight44 : BASE {
	JumboLedLight44() {
		this->box.size = Vec(34.0,34.0);
	}
};

struct JumboLedSwitch44 : app::SvgSwitch {
	JumboLedSwitch44() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_JumboLEDBezel44_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_JumboLEDBezel44_1.svg")));
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
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_PJ301M.svg")));
	}
};

struct as_PJ301MPortGold : app::SvgPort {
	as_PJ301MPortGold() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_PJ301M_output_port.svg")));
	}
};

struct as_SlidePot : app::SvgSlider {
	as_SlidePot() {
		Vec margin = Vec(4, 4);
		maxHandlePos = Vec(-1.5, -8).plus(margin);
		minHandlePos = Vec(-1.5, 87).plus(margin);
		setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/as_SlidePot.svg")));
		background->wrap();
		background->box.pos = margin;
		box.size = background->box.size.plus(margin.mult(2));
		setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/as_SlidePotHandle.svg")));
		handle->wrap();
	}
};

struct as_FaderPot : app::SvgSlider {
	as_FaderPot() {
		Vec margin = Vec(4, 4);
		maxHandlePos = Vec(-1.5, -8).plus(margin);
		minHandlePos = Vec(-1.5, 57).plus(margin);
		setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/as_FaderPot.svg")));
		background->wrap();
		background->box.pos = margin;
		box.size = background->box.size.plus(margin.mult(2));
		setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/as_SlidePotHandle.svg")));
		handle->wrap();
	}
};
