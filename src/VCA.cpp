//**************************************************************************************
//VCA VCV Rack mods by Alfredo Santamaria - AS - http://www.hakken.com.mx
//
//Code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************

#include "AS.hpp"

struct VCA : Module {
	enum ParamIds {
		LEVEL1_PARAM,
		LEVEL2_PARAM,
        MODE1_PARAM,
        MODE2_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		EXP1_INPUT,
		LIN1_INPUT,
		IN1_INPUT,
		EXP2_INPUT,
		LIN2_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	VCA() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step() override;
};


static void stepChannel(Input &in, Param &level, Input &lin, Input &exp, Output &out) {
	float v = in.value * level.value;
	if (lin.active)
		v *= clampf(lin.value / 10.0, 0.0, 1.0);
	const float expBase = 50.0;
	if (exp.active)
		v *= rescalef(powf(expBase, clampf(exp.value / 10.0, 0.0, 1.0)), 1.0, expBase, 0.0, 1.0);
	out.value = v;
}

void VCA::step() {
	stepChannel(inputs[IN1_INPUT], params[LEVEL1_PARAM], inputs[LIN1_INPUT], inputs[EXP1_INPUT], outputs[OUT1_OUTPUT]);
	stepChannel(inputs[IN2_INPUT], params[LEVEL2_PARAM], inputs[LIN2_INPUT], inputs[EXP2_INPUT], outputs[OUT2_OUTPUT]);
}


VCAWidget::VCAWidget() {
	VCA *module = new VCA();
	setModule(module);
    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/VCA.svg")));
		addChild(panel);
	}

	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    //SLIDERS
	addParam(createParam<as_SlidePot>(Vec(10, 70), module, VCA::LEVEL1_PARAM, 0.0, 1.0, 0.5));
	addParam(createParam<as_SlidePot>(Vec(55, 70), module, VCA::LEVEL2_PARAM, 0.0, 1.0, 0.5));
    //MODE SWITCHES
    //addParam(createParam<as_CKSS>(Vec(14, 180), module, VCA::MODE1_PARAM, 0.0, 1.0, 1.0));
	//addParam(createParam<as_CKSS>(Vec(59, 180), module, VCA::MODE2_PARAM, 0.0, 1.0, 1.0));
	addInput(createInput<as_PJ301MPort>(Vec(10, 190), module, VCA::LIN1_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(55, 190), module, VCA::LIN2_INPUT));

	addInput(createInput<as_PJ301MPort>(Vec(10, 217), module, VCA::EXP1_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(55, 217), module, VCA::EXP2_INPUT));

	addInput(createInput<as_PJ301MPort>(Vec(10, 260), module, VCA::IN1_INPUT));
    addInput(createInput<as_PJ301MPort>(Vec(55, 260), module, VCA::IN2_INPUT));

	addOutput(createOutput<as_PJ301MPort>(Vec(10, 310), module, VCA::OUT1_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(55, 310), module, VCA::OUT2_OUTPUT));
}
