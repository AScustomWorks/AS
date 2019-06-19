#include "AS.hpp"

struct BlankPanel4 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	BlankPanel4() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override {
	}
};

struct BlankPanel4Widget : ModuleWidget { 
   BlankPanel4Widget(BlankPanel4 *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Blanks/BlankPanel4.svg")));

		//SCREWS - MOD SPACING FOR RACK WIDTH*4
		addChild(createWidget<as_HexScrew>(Vec(0, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	}
};

Model *modelBlankPanel4 = createModel<BlankPanel4, BlankPanel4Widget>( "BlankPanel4");