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

	BlankPanel4() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

void BlankPanel4::step() 
{
}

BlankPanel4Widget::BlankPanel4Widget() 
{
	BlankPanel4 *module = new BlankPanel4();
	setModule(module);
	box.size = Vec(4 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/BlankPanel4.svg")));
		panel->box.size = box.size;
		addChild(panel);
	}

	//SCREWS - MOD SPACING FOR RACK WIDTH*4
	addChild(createScrew<as_HexScrew>(Vec(0, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
}