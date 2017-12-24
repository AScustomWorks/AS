//**************************************************************************************
//SineOSC VCV Rack - Alfredo Santamaria - AS - http://www.hakken.com.mx
//Is just the tutorial module and nothing else hehe
//
//Code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"

struct SineOSC : Module {
	enum ParamIds {
		PITCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SINE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0;
	float blinkPhase = 0.0;

	SineOSC() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};


void SineOSC::step() {
	// Implement a simple sine oscillator
	float deltaTime = 1.0 / engineGetSampleRate();

	// Compute the frequency from the pitch parameter and input
	float pitch = params[PITCH_PARAM].value;
	pitch += inputs[PITCH_INPUT].value;
	pitch = clampf(pitch, -4.0, 4.0);
	float freq = 440.0 * powf(2.0, pitch);

	// Accumulate the phase
	phase += freq * deltaTime;
	if (phase >= 1.0)
		phase -= 1.0;

	// Compute the sine output
	float sine = sinf(2 * M_PI * phase)+ sinf(2 * M_PI * phase * 2);
	outputs[SINE_OUTPUT].value = 5.0 * sine;

	// Blink light at 1Hz
	blinkPhase += deltaTime;
	if (blinkPhase >= 1.0)
		blinkPhase -= 1.0;
	lights[BLINK_LIGHT].value = (blinkPhase < 0.5) ? 1.0 : 0.0;
}


SineOscWidget::SineOscWidget() {
	SineOSC *module = new SineOSC();
	setModule(module);
	box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/SineOSC.svg")));
		addChild(panel);
	}
	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//PARAMS
	addParam(createParam<as_KnobBlack>(Vec(26, 60), module, SineOSC::PITCH_PARAM, -3.0, 3.0, 0.0));
	//INPUTS
	addInput(createInput<as_PJ301MPort>(Vec(33, 260), module, SineOSC::PITCH_INPUT));
	//OUTPUTS
	addOutput(createOutput<as_PJ301MPort>(Vec(33, 310), module, SineOSC::SINE_OUTPUT));
}
