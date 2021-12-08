//**************************************************************************************
//SawOsc module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code taken from RODENTCAT https://github.com/RODENTCAT/RODENTMODULES
//Code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"

struct SawOsc : Module {
	enum ParamIds {
		PITCH_PARAM,
		BASE_PARAM,
		 PW_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		PW_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OSC_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		FREQ_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0f;
	float blinkPhase = 0.0f;
	float freq = 0.0f;
	int base_freq = 0;

	SawOsc() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SawOsc::PITCH_PARAM, -3.0f, 3.0f, 0.0f, "Value", " V");
		configParam(SawOsc::PW_PARAM, 0.0f, 10.0f, 0.0f, "Modulation", "%", 0.0f, 10.0f);
		//New in V2, config switches info without displaying values
		configSwitch(BASE_PARAM, 0.0f, 1.0f, 1.0f, "Base Frequency (Note)", {"C", "A"});
		//new V2, port labels
		//Inputs
		configInput(PITCH_INPUT, "1 V/octave pitch");
		configInput(PW_INPUT, "Mod CV");
		//Outputs
		configOutput(OSC_OUTPUT, "Wave");
		
	}

	void process(const ProcessArgs &args) override{
		// Implement a simple sine oscillator
		float deltaTime = 1.0f / args.sampleRate;
		// Compute the frequency from the pitch parameter and input
		base_freq = params[BASE_PARAM].getValue();	

		float pitch = params[PITCH_PARAM].getValue();
		pitch += inputs[PITCH_INPUT].getVoltage();
		pitch = clamp(pitch, -3.0f, 6.0f);

		if(base_freq==1){
			//Note A4
			freq = 440.0f * powf(2.0f, pitch);
		}else{
			// Note C4
			freq = 261.626f * powf(2.0f, pitch);
		}

		// Accumulate the phase
		phase += freq * deltaTime;
		if (phase >= 1.0f)
			phase -= 1.0f;

	//Mod param
		//float pw = params[PW_PARAM].getValue()*0.1f+1.0f;
		float pw = rescale(params[PW_PARAM].getValue(), 0.0f, 10.0f, -4.2f, 5.0f)*0.1f+1.0f;

		//Mod input
		float minput = inputs[PW_INPUT].getVoltage()*0.3f;
		//Mod param+input
		float pinput = (pw + minput);

		// Compute the sine output
		//float sine = sinf(2 * M_PI * phase);
		//outputs[SINE_OUTPUT].setVoltage(5.0 * sine);
		
		//saw stuff, original dev says square, but it sounds more like a SAW wave, hence this module name hehe
		float saw = cos(exp(pinput * M_PI * phase));///0.87;
		//dc block
		
		float block_coeff = 1.0f - (2.0f * M_PI * (10.0f / 44100.0f));
		float m_prev_in = 0.0f;
		float m_prev_out = 0.0f;
		m_prev_out = saw - m_prev_in + block_coeff * m_prev_out;
		m_prev_in = saw;

		//outputs[OSC_OUTPUT].setVoltage(5 * saw);
		outputs[OSC_OUTPUT].setVoltage(m_prev_out*5);
		lights[FREQ_LIGHT].value = (outputs[OSC_OUTPUT].value > 0.0f) ? 1.0f : 0.0f;
	}

};


struct SawOscWidget : ModuleWidget { 

	SawOscWidget(SawOsc *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SawOSC.svg")));
	
		//SCREWS - SPECIAL SPACING FOR RACK WIDTH*4
		addChild(createWidget<as_HexScrew>(Vec(0, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//LIGHT
		addChild(createLight<SmallLight<RedLight>>(Vec(7, 57), module, SawOsc::FREQ_LIGHT));
		//PARAMS
		addParam(createParam<as_KnobBlack>(Vec(11, 60), module, SawOsc::PITCH_PARAM));
		addParam(createParam<as_KnobBlack>(Vec(11, 120), module, SawOsc::PW_PARAM));

			//BASE FREQ SWITCH
		addParam(createParam<as_CKSSH>(Vec(18, 220), module, SawOsc::BASE_PARAM));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(18, 180), module, SawOsc::PW_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(18, 260), module, SawOsc::PITCH_INPUT));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(18, 310), module, SawOsc::OSC_OUTPUT));
	}
};


Model *modelSawOsc = createModel<SawOsc, SawOscWidget>("SawOSC");