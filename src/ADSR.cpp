//**************************************************************************************
//ADSR VCV Rack mods by Alfredo Santamaria - AS - http://www.hakken.com.mx
//
//Code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"
#include "dsp/digital.hpp"

struct ADSR : Module {
	enum ParamIds {
		ATTACK_PARAM,
		DECAY_PARAM,
		SUSTAIN_PARAM,
		RELEASE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		ATTACK_INPUT,
		DECAY_INPUT,
		SUSTAIN_INPUT,
		RELEASE_INPUT,
		GATE_INPUT,
		TRIG_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		ENVELOPE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ATTACK_LIGHT,
		DECAY_LIGHT,
		SUSTAIN_LIGHT,
		RELEASE_LIGHT,
		NUM_LIGHTS
	};

	bool decaying = false;
	float env = 0.0;
	SchmittTrigger trigger;

	ADSR() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		trigger.setThresholds(0.0, 1.0);
	}
	void step() override;
};


void ADSR::step() {
	float attack = clampf(params[ATTACK_INPUT].value + inputs[ATTACK_INPUT].value / 10.0, 0.0, 1.0);
	float decay = clampf(params[DECAY_PARAM].value + inputs[DECAY_INPUT].value / 10.0, 0.0, 1.0);
	float sustain = clampf(params[SUSTAIN_PARAM].value + inputs[SUSTAIN_INPUT].value / 10.0, 0.0, 1.0);
	float release = clampf(params[RELEASE_PARAM].value + inputs[RELEASE_PARAM].value / 10.0, 0.0, 1.0);
	// Gate and trigger
	bool gated = inputs[GATE_INPUT].value >= 1.0;
	if (trigger.process(inputs[TRIG_INPUT].value))
		decaying = false;

	const float base = 20000.0;
	const float maxTime = 10.0;
	if (gated) {
		if (decaying) {
			// Decay
			if (decay < 1e-4) {
				env = sustain;
			}
			else {
				env += powf(base, 1 - decay) / maxTime * (sustain - env) / engineGetSampleRate();
			}
		}
		else {
			// Attack
			// Skip ahead if attack is all the way down (infinitely fast)
			if (attack < 1e-4) {
				env = 1.0;
			}
			else {
				env += powf(base, 1 - attack) / maxTime * (1.01 - env) / engineGetSampleRate();
			}
			if (env >= 1.0) {
				env = 1.0;
				decaying = true;
			}
		}
	}
	else {
		// Release
		if (release < 1e-4) {
			env = 0.0;
		}
		else {
			env += powf(base, 1 - release) / maxTime * (0.0 - env) / engineGetSampleRate();
		}
		decaying = false;
	}

	bool sustaining = nearf(env, sustain, 1e-3);
	bool resting = nearf(env, 0.0, 1e-3);

	outputs[ENVELOPE_OUTPUT].value = 10.0 * env;

	// Lights
	lights[ATTACK_LIGHT].value = (gated && !decaying) ? 1.0 : 0.0;
	lights[DECAY_LIGHT].value = (gated && decaying && !sustaining) ? 1.0 : 0.0;
	lights[SUSTAIN_LIGHT].value = (gated && decaying && sustaining) ? 1.0 : 0.0;
	lights[RELEASE_LIGHT].value = (!gated && !resting) ? 1.0 : 0.0;
}


ADSRWidget::ADSRWidget() {
	ADSR *module = new ADSR();
	setModule(module);
	box.size = Vec(RACK_GRID_WIDTH*8, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/ADSR.svg")));
		addChild(panel);
	}

	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	
	static const float posX[4] = {13,39,65,91};
	addChild(createLight<SmallLight<RedLight>>(Vec(posX[0]+6, 74), module, ADSR::ATTACK_LIGHT));
	addChild(createLight<SmallLight<RedLight>>(Vec(posX[1]+6, 74), module, ADSR::DECAY_LIGHT));
	addChild(createLight<SmallLight<RedLight>>(Vec(posX[2]+6, 74), module, ADSR::SUSTAIN_LIGHT));
	addChild(createLight<SmallLight<RedLight>>(Vec(posX[3]+6, 74), module, ADSR::RELEASE_LIGHT));

	addParam(createParam<as_SlidePot>(Vec(posX[0]-3, 90), module, ADSR::ATTACK_PARAM, 0.0, 1.0, 0.5));
	addParam(createParam<as_SlidePot>(Vec(posX[1]-3, 90), module, ADSR::DECAY_PARAM, 0.0, 1.0, 0.5));
	addParam(createParam<as_SlidePot>(Vec(posX[2]-3, 90), module, ADSR::SUSTAIN_PARAM, 0.0, 1.0, 0.5));
	addParam(createParam<as_SlidePot>(Vec(posX[3]-3, 90), module, ADSR::RELEASE_PARAM, 0.0, 1.0, 0.5));

	addInput(createInput<as_PJ301MPort>(Vec(posX[0]-4, 217), module, ADSR::ATTACK_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(posX[1]-4, 217), module, ADSR::DECAY_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(posX[2]-4, 217), module, ADSR::SUSTAIN_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(posX[3]-4, 217), module, ADSR::RELEASE_INPUT));

	addInput(createInput<as_PJ301MPort>(Vec(posX[0]-4, 310), module, ADSR::GATE_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(48, 310), module, ADSR::TRIG_INPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(posX[3]-4, 310), module, ADSR::ENVELOPE_OUTPUT));

}

