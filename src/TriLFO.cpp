#include "AS.hpp"
#include "dsp/digital.hpp"


struct LowFrequencyOscillator {
	float phase = 0.0;
	float pw = 0.5;
	float freq = 1.0;
	bool offset = false;
	bool invert = false;
	SchmittTrigger resetTrigger;
	LowFrequencyOscillator() {
		resetTrigger.setThresholds(0.0, 0.01);
	}
	void setPitch(float pitch) {
		pitch = fminf(pitch, 8.0);
		freq = powf(2.0, pitch);
	}
	void setPulseWidth(float pw_) {
		const float pwMin = 0.01;
		pw = clampf(pw_, pwMin, 1.0 - pwMin);
	}
	void setReset(float reset) {
		if (resetTrigger.process(reset)) {
			phase = 0.0;
		}
	}
	void step(float dt) {
		float deltaPhase = fminf(freq * dt, 0.5);
		phase += deltaPhase;
		if (phase >= 1.0)
			phase -= 1.0;
	}
	float sin() {
		if (offset)
			return 1.0 - cosf(2*M_PI * phase) * (invert ? -1.0 : 1.0);
		else
			return sinf(2*M_PI * phase) * (invert ? -1.0 : 1.0);
	}
	float tri(float x) {
		return 4.0 * fabsf(x - roundf(x));
	}
	float tri() {
		if (offset)
			return tri(invert ? phase - 0.5 : phase);
		else
			return -1.0 + tri(invert ? phase - 0.25 : phase - 0.75);
	}
	float saw(float x) {
		return 2.0 * (x - roundf(x));
	}
	float saw() {
		if (offset)
			return invert ? 2.0 * (1.0 - phase) : 2.0 * phase;
		else
			return saw(phase) * (invert ? -1.0 : 1.0);
	}
	float sqr() {
		float sqr = (phase < pw) ^ invert ? 1.0 : -1.0;
		return offset ? sqr + 1.0 : sqr;
	}
	float light() {
		return sinf(2*M_PI * phase);
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
		PW_PARAM,
		PWM_PARAM,
		//
		NUM_PARAMS
	};
	enum InputIds {
		FM1_INPUT,
		FM2_INPUT,
		RESET1_INPUT,
		RESET2_INPUT,
		RESET3_INPUT,		
		PW_INPUT,
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

	TriLFO() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};


void TriLFO::step() {
	//LFO1
	oscillator1.setPitch(params[FREQ1_PARAM].value + params[FM1_PARAM].value * inputs[FM1_INPUT].value + params[FM2_PARAM].value * inputs[FM2_INPUT].value);
	oscillator1.setPulseWidth(params[PW_PARAM].value + params[PWM_PARAM].value * inputs[PW_INPUT].value / 10.0);
	oscillator1.offset = (params[OFFSET1_PARAM].value > 0.0);
	oscillator1.invert = (params[INVERT1_PARAM].value <= 0.0);
	oscillator1.step(1.0 / engineGetSampleRate());
	oscillator1.setReset(inputs[RESET1_INPUT].value);

	outputs[SIN1_OUTPUT].value = 5.0 * oscillator1.sin();
	outputs[TRI1_OUTPUT].value = 5.0 * oscillator1.tri();
	outputs[SAW1_OUTPUT].value = 5.0 * oscillator1.saw();
	outputs[SQR1_OUTPUT].value = 5.0 * oscillator1.sqr();

	lights[PHASE1_POS_LIGHT].setBrightnessSmooth(fmaxf(0.0, oscillator1.light()));
	lights[PHASE1_NEG_LIGHT].setBrightnessSmooth(fmaxf(0.0, -oscillator1.light()));
	//LFO2
	oscillator2.setPitch(params[FREQ2_PARAM].value + params[FM1_PARAM].value * inputs[FM1_INPUT].value + params[FM2_PARAM].value * inputs[FM2_INPUT].value);
	oscillator2.setPulseWidth(params[PW_PARAM].value + params[PWM_PARAM].value * inputs[PW_INPUT].value / 10.0);
	oscillator2.offset = (params[OFFSET2_PARAM].value > 0.0);
	oscillator2.invert = (params[INVERT2_PARAM].value <= 0.0);
	oscillator2.step(1.0 / engineGetSampleRate());
	oscillator2.setReset(inputs[RESET2_INPUT].value);

	outputs[SIN2_OUTPUT].value = 5.0 * oscillator2.sin();
	outputs[TRI2_OUTPUT].value = 5.0 * oscillator2.tri();
	outputs[SAW2_OUTPUT].value = 5.0 * oscillator2.saw();
	outputs[SQR2_OUTPUT].value = 5.0 * oscillator2.sqr();

	lights[PHASE2_POS_LIGHT].setBrightnessSmooth(fmaxf(0.0, oscillator2.light()));
	lights[PHASE2_NEG_LIGHT].setBrightnessSmooth(fmaxf(0.0, -oscillator2.light()));
	//LFO3
	oscillator3.setPitch(params[FREQ3_PARAM].value + params[FM1_PARAM].value * inputs[FM1_INPUT].value + params[FM2_PARAM].value * inputs[FM2_INPUT].value);
	oscillator3.setPulseWidth(params[PW_PARAM].value + params[PWM_PARAM].value * inputs[PW_INPUT].value / 10.0);
	oscillator3.offset = (params[OFFSET3_PARAM].value > 0.0);
	oscillator3.invert = (params[INVERT3_PARAM].value <= 0.0);
	oscillator3.step(1.0 / engineGetSampleRate());
	oscillator3.setReset(inputs[RESET3_INPUT].value);

	outputs[SIN3_OUTPUT].value = 5.0 * oscillator3.sin();
	outputs[TRI3_OUTPUT].value = 5.0 * oscillator3.tri();
	outputs[SAW3_OUTPUT].value = 5.0 * oscillator3.saw();
	outputs[SQR3_OUTPUT].value = 5.0 * oscillator3.sqr();

	lights[PHASE3_POS_LIGHT].setBrightnessSmooth(fmaxf(0.0, oscillator3.light()));
	lights[PHASE3_NEG_LIGHT].setBrightnessSmooth(fmaxf(0.0, -oscillator3.light()));

}


TriLFOWidget::TriLFOWidget() {
	TriLFO *module = new TriLFO();
	setModule(module);
	box.size = Vec(RACK_GRID_WIDTH*10, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/as_LFO.svg")));
		addChild(panel);
	}

 	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//LFO 1
	addInput(createInput<as_PJ301MPort>(Vec(10, 60), module, TriLFO::RESET1_INPUT));
	addParam(createParam<as_KnobBlack>(Vec(41, 55), module, TriLFO::FREQ1_PARAM, -8.0, 6.0, -1.0));
	//
    addChild(createLight<SmallLight<GreenRedLight>>(Vec(37, 52), module, TriLFO::PHASE1_POS_LIGHT));
	//
    addParam(createParam<as_CKSS>(Vec(90, 60), module, TriLFO::OFFSET1_PARAM, 0.0, 1.0, 1.0));
	addParam(createParam<as_CKSS>(Vec(120, 60), module, TriLFO::INVERT1_PARAM, 0.0, 1.0, 1.0));
	//
	addOutput(createOutput<as_PJ301MPort>(Vec(11, 120), module, TriLFO::SIN1_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(45, 120), module, TriLFO::TRI1_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(80, 120), module, TriLFO::SAW1_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(114, 120), module, TriLFO::SQR1_OUTPUT));
	//LFO 2
	static const float lfo2_y_offset = 100;
	addInput(createInput<as_PJ301MPort>(Vec(10, 60+lfo2_y_offset), module, TriLFO::RESET2_INPUT));
	addParam(createParam<as_KnobBlack>(Vec(41, 55+lfo2_y_offset), module, TriLFO::FREQ2_PARAM, -8.0, 6.0, -1.0));
	//
    addChild(createLight<SmallLight<GreenRedLight>>(Vec(37, 52+lfo2_y_offset), module, TriLFO::PHASE2_POS_LIGHT));
	//
    addParam(createParam<as_CKSS>(Vec(90, 60+lfo2_y_offset), module, TriLFO::OFFSET2_PARAM, 0.0, 1.0, 1.0));
	addParam(createParam<as_CKSS>(Vec(120, 60+lfo2_y_offset), module, TriLFO::INVERT2_PARAM, 0.0, 1.0, 1.0));
	//
	addOutput(createOutput<as_PJ301MPort>(Vec(11, 120+lfo2_y_offset), module, TriLFO::SIN2_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(45, 120+lfo2_y_offset), module, TriLFO::TRI2_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(80, 120+lfo2_y_offset), module, TriLFO::SAW2_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(114, 120+lfo2_y_offset), module, TriLFO::SQR2_OUTPUT));
	//LFO 3
	static const float lfo3_y_offset = 200;
	addInput(createInput<as_PJ301MPort>(Vec(10, 60+lfo3_y_offset), module, TriLFO::RESET3_INPUT));
	addParam(createParam<as_KnobBlack>(Vec(41, 55+lfo3_y_offset), module, TriLFO::FREQ3_PARAM, -8.0, 6.0, -1.0));
	//
    addChild(createLight<SmallLight<GreenRedLight>>(Vec(37, 52+lfo3_y_offset), module, TriLFO::PHASE3_POS_LIGHT));
	//
    addParam(createParam<as_CKSS>(Vec(90, 60+lfo3_y_offset), module, TriLFO::OFFSET3_PARAM, 0.0, 1.0, 1.0));
	addParam(createParam<as_CKSS>(Vec(120, 60+lfo3_y_offset), module, TriLFO::INVERT3_PARAM, 0.0, 1.0, 1.0));
	//
	addOutput(createOutput<as_PJ301MPort>(Vec(11, 120+lfo3_y_offset), module, TriLFO::SIN3_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(45, 120+lfo3_y_offset), module, TriLFO::TRI3_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(80, 120+lfo3_y_offset), module, TriLFO::SAW3_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(114, 120+lfo3_y_offset), module, TriLFO::SQR3_OUTPUT));

    /*
	addParam(createParam<as_KnobBlack>(Vec(23, 143), module, TriLFO::FM1_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<as_KnobBlack>(Vec(91, 143), module, TriLFO::PW_PARAM, 0.0, 1.0, 0.5));
	addParam(createParam<as_KnobBlack>(Vec(23, 208), module, TriLFO::FM2_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<as_KnobBlack>(Vec(91, 208), module, TriLFO::PWM_PARAM, 0.0, 1.0, 0.0));
*/
/*
	addInput(createInput<as_PJ301MPort>(Vec(11, 276), module, TriLFO::FM1_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(45, 276), module, TriLFO::FM2_INPUT));
    */
	//addInput(createInput<as_PJ301MPort>(Vec(114, 276), module, TriLFO::PW_INPUT));
/*
	addOutput(createOutput<as_PJ301MPort>(Vec(11, 320), module, TriLFO::SIN3_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(45, 320), module, TriLFO::TRI3_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(80, 320), module, TriLFO::SAW3_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(114, 320), module, TriLFO::SQR3_OUTPUT));
*/
}