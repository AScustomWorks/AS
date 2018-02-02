//***********************************************************************************************
//
//TremoloFx module for VCV Rack by Alfredo Santamaria  - AS - https://github.com/AScustomWorks/AS
//
//LFO code adapted from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//***********************************************************************************************

#include "AS.hpp"
#include "dsp/digital.hpp"

//LFO CODE *****************************
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
	/*
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
	*/
	float light() {
		return sinf(2*M_PI * phase);
	}
};
//LFO CODE *****************************

struct TremoloFx : Module{
	enum ParamIds {
		WAVE_PARAM,
		FREQ_PARAM,
		BLEND_PARAM,
		INVERT_PARAM,
        BYPASS_SWITCH,
		NUM_PARAMS
	};
	enum InputIds {
		SIGNAL_INPUT,
		WAVE_CV_INPUT,
		FREQ_CV_INPUT,
		BLEND_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SIGNAL_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		WAVE_LIGHT,
		PHASE_POS_LIGHT,
		PHASE_NEG_LIGHT,
		BLEND_LIGHT,
		BYPASS_LED,
		NUM_LIGHTS
	};

	LowFrequencyOscillator oscillator;

	SchmittTrigger bypass_button_trig;

	bool fx_bypass = false;

	TremoloFx() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}

	void step() override;

	json_t *toJson()override {
		json_t *rootJm = json_object();

		json_t *statesJ = json_array();
		
			json_t *bypassJ = json_boolean(fx_bypass);
			json_array_append_new(statesJ, bypassJ);
		
		json_object_set_new(rootJm, "as_FxBypass", statesJ);

		return rootJm;
	}

	void fromJson(json_t *rootJm)override {
		json_t *statesJ = json_object_get(rootJm, "as_FxBypass");
		
			json_t *bypassJ = json_array_get(statesJ, 0);

			fx_bypass = !!json_boolean_value(bypassJ);
		
	}
	
	float input_signal =0.0;
	float output_signal = 0.0;
	float tremolo_signal = 0.0;
	float blend_control = 0.0;
	float lfo_modulation = 0.0;
	
};

void TremoloFx::step() {

	if (bypass_button_trig.process(params[BYPASS_SWITCH].value)){
		  fx_bypass = !fx_bypass;
	}
    lights[BYPASS_LED].value = fx_bypass ? 1.0 : 0.0;

	input_signal = clampf(inputs[SIGNAL_INPUT].value,-10.0,10.0);

	//oscillator.setPitch(params[FREQ_PARAM].value);
	oscillator.setPitch( clampf(params[FREQ_PARAM].value + inputs[FREQ_CV_INPUT].value, 0.0, 3.5) );
	oscillator.offset = (1.0);
	oscillator.invert = (params[INVERT_PARAM].value <= 0.0);
	oscillator.setPulseWidth(0.5);
	oscillator.step(1.0 / engineGetSampleRate());

	float wave = clampf( params[WAVE_PARAM].value + inputs[WAVE_CV_INPUT].value, 0.0, 1.0 );
	float interp = crossf(oscillator.sin(), oscillator.tri(), wave);

	lfo_modulation = 5.0 * interp;

	//check bypass switch status
	if (fx_bypass){
		outputs[SIGNAL_OUTPUT].value = input_signal;
	}else {
		tremolo_signal = input_signal * clampf(lfo_modulation/10, 0.0, 1.0);
		blend_control = clampf(params[BLEND_PARAM].value + inputs[BLEND_CV_INPUT].value / 10.0, 0.0, 1.0);
		output_signal = crossf(input_signal,tremolo_signal,blend_control);
		outputs[SIGNAL_OUTPUT].value = output_signal;
	}

	lights[PHASE_POS_LIGHT].setBrightnessSmooth(fmaxf(0.0, oscillator.light()));
	lights[PHASE_NEG_LIGHT].setBrightnessSmooth(fmaxf(0.0, -oscillator.light()));
	lights[BLEND_LIGHT].value = clampf(params[BLEND_PARAM].value + inputs[BLEND_CV_INPUT].value / 10.0, 0.0, 1.0);

}

TremoloFxWidget::TremoloFxWidget() {
	TremoloFx *module = new TremoloFx();
	setModule(module);
    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;	
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/Tremolo.svg")));
		addChild(panel); 
	}

 	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//phase switch
	addParam(createParam<as_CKSS>(Vec(13, 100), module, TremoloFx::INVERT_PARAM, 0.0, 1.0, 1.0));
    //KNOBS  
	addParam(createParam<as_FxKnobWhite>(Vec(43, 60), module, TremoloFx::WAVE_PARAM, 0.0, 1.0, 0.5));
	addParam(createParam<as_FxKnobWhite>(Vec(43, 125), module, TremoloFx::FREQ_PARAM, 0, 3.5, 1.75));
	addParam(createParam<as_FxKnobWhite>(Vec(43, 190), module, TremoloFx::BLEND_PARAM, 0.0, 1.0, 0.5));
	//LIGHTS
	addChild(createLight<SmallLight<YellowRedLight>>(Vec(39, 122), module, TremoloFx::PHASE_POS_LIGHT));
	addChild(createLight<SmallLight<YellowLight>>(Vec(39, 187), module, TremoloFx::BLEND_LIGHT));
    //BYPASS SWITCH
  	addParam(createParam<LEDBezel>(Vec(33, 260), module, TremoloFx::BYPASS_SWITCH , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(35.2, 262), module, TremoloFx::BYPASS_LED));
    //INS/OUTS
	addInput(createInput<as_PJ301MPort>(Vec(10, 310), module, TremoloFx::SIGNAL_INPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(55, 310), module, TremoloFx::SIGNAL_OUTPUT));
	//CV INPUTS
	addInput(createInput<as_PJ301MPort>(Vec(10, 67), module, TremoloFx::WAVE_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(10, 132), module, TremoloFx::FREQ_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(10, 197), module, TremoloFx::BLEND_CV_INPUT));
 
}
