//**************************************************************************************
//ADSR module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"
//#include "dsp/digital.hpp"

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
	float env = 0.0f;
	dsp::SchmittTrigger trigger;

	ADSR() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(ADSR::ATTACK_PARAM, 0.0f, 1.0f, 0.5f, "Attack", "%", 0.0f, 100.0f);
		configParam(ADSR::DECAY_PARAM, 0.0f, 1.0f, 0.5f, "Decay", "%", 0.0f, 100.0f);
		configParam(ADSR::SUSTAIN_PARAM, 0.0f, 1.0f, 0.5f, "Sustain", "%", 0.0f, 100.0f);
		configParam(ADSR::RELEASE_PARAM, 0.0f, 1.0f, 0.5f, "Release", "%", 0.0f, 100.0f);

		//new V2, port labels
		//inputs
		configInput(TRIG_INPUT, "Retrigger");
		configInput(GATE_INPUT, "Gate");
		configInput(ATTACK_INPUT, "Attack CV");
		configInput(SUSTAIN_INPUT, "Sustain CV");
		configInput(DECAY_INPUT, "Decay CV");
		configInput(RELEASE_INPUT, "Release CV");

		//Outputs
		configOutput(ENVELOPE_OUTPUT, "Envelope");

	}
	void process(const ProcessArgs &args) override {
		float attack = clamp(params[ATTACK_PARAM].getValue() + inputs[ATTACK_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		float decay = clamp(params[DECAY_PARAM].getValue() + inputs[DECAY_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		float sustain = clamp(params[SUSTAIN_PARAM].getValue() + inputs[SUSTAIN_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		float release = clamp(params[RELEASE_PARAM].getValue() + inputs[RELEASE_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		// Gate and trigger
		bool gated = inputs[GATE_INPUT].getVoltage() >= 1.0f;
		if (trigger.process(inputs[TRIG_INPUT].getVoltage()))
			decaying = false;

		const float base = 20000.0f;
		const float maxTime = 10.0f;
		if (gated) {
			if (decaying) {
				// Decay
				if (decay < 1e-4) {
					env = sustain;
				}
				else {
					env += powf(base, 1 - decay) / maxTime * (sustain - env) / args.sampleRate;
				}
			}
			else {
				// Attack
				// Skip ahead if attack is all the way down (infinitely fast)
				if (attack < 1e-4) {
					env = 1.0f;
				}
				else {
					env += powf(base, 1 - attack) / maxTime * (1.01 - env) / args.sampleRate;
				}
				if (env >= 1.0f) {
					env = 1.0f;
					decaying = true;
				}
			}
		}
		else {
			// Release
			if (release < 1e-4) {
				env = 0.0f;
			}
			else {
				env += powf(base, 1 - release) / maxTime * (0.0 - env) / args.sampleRate;
			}
			decaying = false;
		}

		bool sustaining = isNear(env, sustain, 1e-3);
		bool resting = isNear(env, 0.0, 1e-3);

		outputs[ENVELOPE_OUTPUT].setVoltage(10.0f * env);

		// Lights
		lights[ATTACK_LIGHT].value = (gated && !decaying) ? 1.0f : 0.0f;
		lights[DECAY_LIGHT].value = (gated && decaying && !sustaining) ? 1.0f : 0.0f;
		lights[SUSTAIN_LIGHT].value = (gated && decaying && sustaining) ? 1.0f : 0.0f;
		lights[RELEASE_LIGHT].value = (!gated && !resting) ? 1.0f : 0.0f;
	}
};



struct ADSRWidget : ModuleWidget { 
   	 ADSRWidget(ADSR *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ADSR.svg")));

		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		static const float posX[4] = {13.0f,39.0f,65.0f,91.0f};
		addChild(createLight<SmallLight<RedLight>>(Vec(posX[0]+6, 74), module, ADSR::ATTACK_LIGHT));
		addChild(createLight<SmallLight<RedLight>>(Vec(posX[1]+6, 74), module, ADSR::DECAY_LIGHT));
		addChild(createLight<SmallLight<RedLight>>(Vec(posX[2]+6, 74), module, ADSR::SUSTAIN_LIGHT));
		addChild(createLight<SmallLight<RedLight>>(Vec(posX[3]+6, 74), module, ADSR::RELEASE_LIGHT));

		addParam(createParam<as_SlidePot>(Vec(posX[0]-3, 90), module, ADSR::ATTACK_PARAM));
		addParam(createParam<as_SlidePot>(Vec(posX[1]-3, 90), module, ADSR::DECAY_PARAM));
		addParam(createParam<as_SlidePot>(Vec(posX[2]-3, 90), module, ADSR::SUSTAIN_PARAM));
		addParam(createParam<as_SlidePot>(Vec(posX[3]-3, 90), module, ADSR::RELEASE_PARAM));

		addInput(createInput<as_PJ301MPort>(Vec(posX[0]-4, 217), module, ADSR::ATTACK_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(posX[1]-4, 217), module, ADSR::DECAY_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(posX[2]-4, 217), module, ADSR::SUSTAIN_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(posX[3]-4, 217), module, ADSR::RELEASE_INPUT));

		addInput(createInput<as_PJ301MPort>(Vec(posX[0]-4, 310), module, ADSR::GATE_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(48, 310), module, ADSR::TRIG_INPUT));

		addOutput(createOutput<as_PJ301MPortGold>(Vec(posX[3]-4, 310), module, ADSR::ENVELOPE_OUTPUT));
	}
};

Model *modelADSR = createModel<ADSR, ADSRWidget>("ADSR");