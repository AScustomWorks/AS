//**************************************************************************************
//TriLFO module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code adapted from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"
//#include "dsp/digital.hpp"

struct LowFrequencyOscillator {
	float phase = 0.0f;
	float pw = 0.5f;
	float freq = 1.0f;
	bool offset = false;
	bool invert = false;
	dsp::SchmittTrigger resetTrigger;
	LowFrequencyOscillator() {
		//resetTrigger.setThresholds(0.0f, 0.01f);
	}
	void setPitch(float pitch) {
		pitch = fminf(pitch, 8.0f);
		freq = powf(2.0f, pitch);
	}
	void setPulseWidth(float pw_) {
		const float pwMin = 0.01f;
		pw = clamp(pw_, pwMin, 1.0f - pwMin);
	}
	void setReset(float reset) {
		if (resetTrigger.process(reset)) {
			phase = 0.0f;
		}
	}
	void step(float dt) {
		float deltaPhase = fminf(freq * dt, 0.5f);
		phase += deltaPhase;
		if (phase >= 1.0f)
			phase -= 1.0f;
	}
	float sin() {
		if (offset)
			return 1.0f - cosf(2*M_PI * phase) * (invert ? -1.0f : 1.0f);
		else
			return sinf(2.0f*M_PI * phase) * (invert ? -1.0f : 1.0f);
	}
	float tri(float x) {
		return 4.0f * fabsf(x - roundf(x));
	}
	float tri() {
		if (offset)
			return tri(invert ? phase - 0.5f : phase);
		else
			return -1.0f + tri(invert ? phase - 0.25f : phase - 0.75f);
	}
	float saw(float x) {
		return 2.0f * (x - roundf(x));
	}
	float saw() {
		if (offset)
			return invert ? 2.0f * (1.0f - phase) : 2.0f * phase;
		else
			return saw(phase) * (invert ? -1.0f : 1.0f);
	}
	float sqr() {
		float sqr = (phase < pw) ^ invert ? 1.0f : -1.0f;
		return offset ? sqr + 1.0f : sqr;
	}
	float light() {
		return sinf(2.0f*M_PI * phase);
	}
};


struct TriLFO : Module {
	enum ParamIds {
		OFFSET1_PARAM,
		INVERT1_PARAM,
		FREQ1_PARAM,
		OFFSET2_PARAM,
		INVERT2_PARAM,
		FREQ2_PARAM,
		OFFSET3_PARAM,
		INVERT3_PARAM,
		FREQ3_PARAM,
		//
		FM1_PARAM,
		FM2_PARAM,
		//
		NUM_PARAMS
	};
	enum InputIds {
		RESET1_INPUT,
		RESET2_INPUT,
		RESET3_INPUT,		
		NUM_INPUTS
	};
	enum OutputIds {
		SIN1_OUTPUT,
		TRI1_OUTPUT,
		SAW1_OUTPUT,
		SQR1_OUTPUT,
		SIN2_OUTPUT,
		TRI2_OUTPUT,
		SAW2_OUTPUT,
		SQR2_OUTPUT,
		SIN3_OUTPUT,
		TRI3_OUTPUT,
		SAW3_OUTPUT,
		SQR3_OUTPUT,		
		NUM_OUTPUTS
	};
	enum LightIds {
		PHASE1_POS_LIGHT,
		PHASE1_NEG_LIGHT,
		PHASE2_POS_LIGHT,
		PHASE2_NEG_LIGHT,
		PHASE3_POS_LIGHT,
		PHASE3_NEG_LIGHT,
		NUM_LIGHTS
	};

	LowFrequencyOscillator oscillator1;
	LowFrequencyOscillator oscillator2;
	LowFrequencyOscillator oscillator3;

	TriLFO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(TriLFO::FREQ1_PARAM, -8.0f, 10.0f, -1.0f, "CH 1 Frequency", " Hz", 2, 1);
		configParam(TriLFO::FREQ2_PARAM, -8.0f, 10.0f, -1.0f, "CH 2 Frequency", " Hz", 2, 1);
		configParam(TriLFO::FREQ3_PARAM, -8.0f, 10.0f, -1.0f, "CH 3 Frequency", " Hz", 2, 1);

		//New in V2, config switches and ports info without displaying values
		configSwitch(OFFSET1_PARAM, 0.0f, 1.0f, 1.0f, "CH 1 Offset", {"Bipolar", "Uniolar"});
		configSwitch(OFFSET2_PARAM, 0.0f, 1.0f, 1.0f, "CH 2 Offset", {"Bipolar", "Uniolar"});
		configSwitch(OFFSET3_PARAM, 0.0f, 1.0f, 1.0f, "CH 3 Offset", {"Bipolar", "Uniolar"});
		configSwitch(INVERT1_PARAM,  0.0f, 1.0f, 1.0f, "CH 1 Invert", {"180º", "0º"});
		configSwitch(INVERT2_PARAM,  0.0f, 1.0f, 1.0f, "CH 2 Invert", {"180º", "0º"});
		configSwitch(INVERT3_PARAM,  0.0f, 1.0f, 1.0f, "CH 2 Invert", {"180º", "0º"});
		//inputs
		configInput(RESET1_INPUT, "CH 1 Reset CV");
		configInput(RESET2_INPUT, "CH 2 Reset CV");
		configInput(RESET3_INPUT, "CH 3 Reset CV");
		//Outputs
		configOutput(SIN1_OUTPUT, "Sine 1");
		configOutput(TRI1_OUTPUT, "Triangle 1");
		configOutput(SAW1_OUTPUT, "Saw 1");
		configOutput(SQR1_OUTPUT, "Square 1");	
		configOutput(SIN2_OUTPUT, "Sine 2");
		configOutput(TRI2_OUTPUT, "Triangle 2");
		configOutput(SAW2_OUTPUT, "Saw 2");
		configOutput(SQR2_OUTPUT, "Square 2");
		configOutput(SIN3_OUTPUT, "Sine 3");
		configOutput(TRI3_OUTPUT, "Triangle 3");
		configOutput(SAW3_OUTPUT, "Saw 3");
		configOutput(SQR3_OUTPUT, "Square 3");		

	}

	float pw_param = 0.5f;

	void process(const ProcessArgs &args) override {
		//LFO1
		oscillator1.setPitch(params[FREQ1_PARAM].getValue());
		oscillator1.setPulseWidth(pw_param);
		oscillator1.offset = (params[OFFSET1_PARAM].getValue() > 0.0f);
		oscillator1.invert = (params[INVERT1_PARAM].getValue() <= 0.0f);
		oscillator1.step(1.0f / args.sampleRate);
		oscillator1.setReset(inputs[RESET1_INPUT].getVoltage());

		outputs[SIN1_OUTPUT].setVoltage(5.0f * oscillator1.sin());
		outputs[TRI1_OUTPUT].setVoltage(5.0f * oscillator1.tri());
		outputs[SAW1_OUTPUT].setVoltage(5.0f * oscillator1.saw());
		outputs[SQR1_OUTPUT].setVoltage(5.0f * oscillator1.sqr());

		lights[PHASE1_POS_LIGHT].setSmoothBrightness(fmaxf(0.0f, oscillator1.light()), args.sampleTime);
		lights[PHASE1_NEG_LIGHT].setSmoothBrightness(fmaxf(0.0f, -oscillator1.light()), args.sampleTime);
		//LFO2
		oscillator2.setPitch(params[FREQ2_PARAM].getValue());
		oscillator2.setPulseWidth(pw_param);
		oscillator2.offset = (params[OFFSET2_PARAM].getValue() > 0.0f);
		oscillator2.invert = (params[INVERT2_PARAM].getValue() <= 0.0f);
		oscillator2.step(1.0f / args.sampleRate);
		oscillator2.setReset(inputs[RESET2_INPUT].getVoltage());

		outputs[SIN2_OUTPUT].setVoltage(5.0f * oscillator2.sin());
		outputs[TRI2_OUTPUT].setVoltage(5.0f * oscillator2.tri());
		outputs[SAW2_OUTPUT].setVoltage(5.0f * oscillator2.saw());
		outputs[SQR2_OUTPUT].setVoltage(5.0f * oscillator2.sqr());

		lights[PHASE2_POS_LIGHT].setSmoothBrightness(fmaxf(0.0f, oscillator2.light()), args.sampleTime);
		lights[PHASE2_NEG_LIGHT].setSmoothBrightness(fmaxf(0.0f, -oscillator2.light()), args.sampleTime);
		//LFO3
		oscillator3.setPitch(params[FREQ3_PARAM].getValue());
		oscillator3.setPulseWidth(pw_param);
		oscillator3.offset = (params[OFFSET3_PARAM].getValue() > 0.0f);
		oscillator3.invert = (params[INVERT3_PARAM].getValue() <= 0.0f);
		oscillator3.step(1.0f / args.sampleRate);
		oscillator3.setReset(inputs[RESET3_INPUT].getVoltage());

		outputs[SIN3_OUTPUT].setVoltage(5.0f * oscillator3.sin());
		outputs[TRI3_OUTPUT].setVoltage(5.0f * oscillator3.tri());
		outputs[SAW3_OUTPUT].setVoltage(5.0f * oscillator3.saw());
		outputs[SQR3_OUTPUT].setVoltage(5.0f * oscillator3.sqr());

		lights[PHASE3_POS_LIGHT].setSmoothBrightness(fmaxf(0.0f, oscillator3.light()), args.sampleTime);
		lights[PHASE3_NEG_LIGHT].setSmoothBrightness(fmaxf(0.0f, -oscillator3.light()), args.sampleTime);

	}

};



struct TriLFOWidget : ModuleWidget { 

	TriLFOWidget(TriLFO *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/as_LFO.svg")));
	
		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//LFO 1
		addInput(createInput<as_PJ301MPort>(Vec(10, 60), module, TriLFO::RESET1_INPUT));
		addParam(createParam<as_KnobBlack>(Vec(41, 55), module, TriLFO::FREQ1_PARAM));
		//
		addChild(createLight<SmallLight<GreenRedLight>>(Vec(37, 52), module, TriLFO::PHASE1_POS_LIGHT));
		//
		addParam(createParam<as_CKSS>(Vec(90, 60), module, TriLFO::OFFSET1_PARAM));
		addParam(createParam<as_CKSS>(Vec(120, 60), module, TriLFO::INVERT1_PARAM));
		//
		addOutput(createOutput<as_PJ301MPortGold>(Vec(11, 120), module, TriLFO::SIN1_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(45, 120), module, TriLFO::TRI1_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(80, 120), module, TriLFO::SAW1_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(114, 120), module, TriLFO::SQR1_OUTPUT));
		//LFO 2
		static const int lfo2_y_offset = 100;
		addInput(createInput<as_PJ301MPort>(Vec(10, 60+lfo2_y_offset), module, TriLFO::RESET2_INPUT));
		addParam(createParam<as_KnobBlack>(Vec(41, 55+lfo2_y_offset), module, TriLFO::FREQ2_PARAM));
		//
		addChild(createLight<SmallLight<GreenRedLight>>(Vec(37, 52+lfo2_y_offset), module, TriLFO::PHASE2_POS_LIGHT));
		//
		addParam(createParam<as_CKSS>(Vec(90, 60+lfo2_y_offset), module, TriLFO::OFFSET2_PARAM));
		addParam(createParam<as_CKSS>(Vec(120, 60+lfo2_y_offset), module, TriLFO::INVERT2_PARAM));
		//
		addOutput(createOutput<as_PJ301MPortGold>(Vec(11, 120+lfo2_y_offset), module, TriLFO::SIN2_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(45, 120+lfo2_y_offset), module, TriLFO::TRI2_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(80, 120+lfo2_y_offset), module, TriLFO::SAW2_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(114, 120+lfo2_y_offset), module, TriLFO::SQR2_OUTPUT));
		//LFO 3
		static const int lfo3_y_offset = 200;
		addInput(createInput<as_PJ301MPort>(Vec(10, 60+lfo3_y_offset), module, TriLFO::RESET3_INPUT));
		addParam(createParam<as_KnobBlack>(Vec(41, 55+lfo3_y_offset), module, TriLFO::FREQ3_PARAM));
		//
		addChild(createLight<SmallLight<GreenRedLight>>(Vec(37, 52+lfo3_y_offset), module, TriLFO::PHASE3_POS_LIGHT));
		//
		addParam(createParam<as_CKSS>(Vec(90, 60+lfo3_y_offset), module, TriLFO::OFFSET3_PARAM));
		addParam(createParam<as_CKSS>(Vec(120, 60+lfo3_y_offset), module, TriLFO::INVERT3_PARAM));
		//
		addOutput(createOutput<as_PJ301MPortGold>(Vec(11, 120+lfo3_y_offset), module, TriLFO::SIN3_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(45, 120+lfo3_y_offset), module, TriLFO::TRI3_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(80, 120+lfo3_y_offset), module, TriLFO::SAW3_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(114, 120+lfo3_y_offset), module, TriLFO::SQR3_OUTPUT));

	}
};


Model *modelTriLFO = createModel<TriLFO, TriLFOWidget>("TriLFO");