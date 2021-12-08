//**************************************************************************************
//SineOSC module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//Is just the tutorial module and nothing else hehe
//
//Code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"

struct SineOsc : Module {
	enum ParamIds {
		FREQ_PARAM,
		BASE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		FREQ_CV,
		NUM_INPUTS
	};
	enum OutputIds {
		OSC_OUTPUT,
		TRI_OUTPUT,
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

	SineOsc() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SineOsc::FREQ_PARAM, -3.0f, 3.0f, 0.0f, "Value", " V");
		configParam(SineOsc::BASE_PARAM, 0.0f, 1.0f, 1.0f, "Base Frequency: A - C");

		//New in V2, config switches info without displaying values
		configSwitch(BASE_PARAM, 0.0f, 1.0f, 1.0f, "Base Frequency (Note)", {"C", "A"});
		//new V2, port labels
		//Inputs
		configInput(FREQ_CV, "1 V/octave pitch");
		//Outputs
		configOutput(OSC_OUTPUT, "Wave");

	}
	void process(const ProcessArgs &args) override;
};


void SineOsc::process(const ProcessArgs &args) {
	// Implement a simple sine oscillator
	// Compute the frequency from the pitch parameter and input
	base_freq = params[BASE_PARAM].getValue();
	float pitch = params[FREQ_PARAM].getValue();
	pitch += inputs[FREQ_CV].getVoltage();
	pitch = clamp(pitch, -3.0f, 6.0f);

	if(base_freq==1){
		//Note A4
		freq = 440.0f * powf(2.0f, pitch);
	}else{
		// Note C4
		freq = 261.626f * powf(2.0f, pitch);
	}
	// Accumulate the phase
	phase += freq / args.sampleRate;
	if (phase >= 1.0f)
		phase -= 1.0f;
	// Compute the sine output
	//correct sine
	float sine = sinf(2.0f * M_PI * (phase+1 * 0.125f)) * 5.0f;
	//original sine
	//float sine = sinf(2 * M_PI * phase)+ sinf(2 * M_PI * phase * 2)*5;
	//mod,like this it gives  a unipolar saw-ish wave
	//float sine = sinf(2.0 * M_PI * (phase * 0.125)) * 5.0;

	outputs[OSC_OUTPUT].setVoltage(sine);
    lights[FREQ_LIGHT].value = (outputs[OSC_OUTPUT].value > 0.0f) ? 1.0f : 0.0f;

}

struct SineOscWidget : ModuleWidget { 

	SineOscWidget(SineOsc *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SineOSC.svg")));
	
		//SCREWS - SPECIAL SPACING FOR RACK WIDTH*4
		addChild(createWidget<as_HexScrew>(Vec(0, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//LIGHT
		addChild(createLight<SmallLight<RedLight>>(Vec(7, 57), module, SineOsc::FREQ_LIGHT));
		//PARAMS
		addParam(createParam<as_KnobBlack>(Vec(11, 60), module, SineOsc::FREQ_PARAM));

		//BASE FREQ SWITCH
		addParam(createParam<as_CKSSH>(Vec(18, 220), module, SineOsc::BASE_PARAM));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(18, 260), module, SineOsc::FREQ_CV));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(18, 310), module, SineOsc::OSC_OUTPUT));
		
	}
};


Model *modelSineOsc = createModel<SineOsc, SineOscWidget>("SineOSC");