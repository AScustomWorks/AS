//***********************************************************************************************
//
//TremoloFx module for VCV Rack by Alfredo Santamaria  - AS - https://github.com/AScustomWorks/AS
//
//LFO code adapted from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//***********************************************************************************************

#include "AS.hpp"
//#include "dsp/digital.hpp"

//LFO CODE *****************************
struct LowFrequencyOscillator {
	float phase = 0.0f;
	float pw = 0.5f;
	float freq = 1.0f;
	bool offset = false;
	bool invert = false;
	dsp::SchmittTrigger resetTrigger;
	LowFrequencyOscillator() {

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
			return tri(invert ? phase - 0.5 : phase);
		else
			return -1.0f + tri(invert ? phase - 0.25f : phase - 0.75f);
	}
	float light() {
		return sinf(2.0f*M_PI * phase);
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
		BYPASS_CV_INPUT,
		RESET_CV_INPUT,
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

	dsp::SchmittTrigger bypass_button_trig;
	dsp::SchmittTrigger bypass_cv_trig;

	bool fx_bypass = false;

	float fade_in_fx = 0.0f;
	float fade_in_dry = 0.0f;
	float fade_out_fx = 1.0f;
	float fade_out_dry = 1.0f;
    const float fade_speed = 0.001f;

	float input_signal = 0.0f;
	float output_signal = 0.0f;
	float tremolo_signal = 0.0f;
	float blend_control = 0.0f;
	float lfo_modulation = 0.0f;

	TremoloFx() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(TremoloFx::INVERT_PARAM, 0.0f, 1.0f, 1.0f, "Shape Phase Invert");
		configParam(TremoloFx::WAVE_PARAM, 0.0f, 1.0f, 0.5f, "Shape", "%", 0.0f, 100.0f);
		configParam(TremoloFx::FREQ_PARAM, 0.0f, 1.0f, 0.5f, "Speed", "%", 0.0f, 100.0f);
		configParam(TremoloFx::BLEND_PARAM, 0.0f, 1.0f, 0.5f, "Blend", "%", 0.0f, 100.0f);

		//New in V2, config switches info without displaying values
		configSwitch(INVERT_PARAM, 0.0f, 1.0f, 0.0f, "Shape Phase Invert", {"ˆ", "ˇ"});
		//New in V2, config temporary buttons info without displaying values
		configButton(BYPASS_SWITCH, "Bypass");
		//new V2, port labels
		//Inputs
		configInput(WAVE_CV_INPUT, "Wave CV");
		configInput(FREQ_CV_INPUT, "Speed CV");
		configInput(BLEND_CV_INPUT, "Blend CV");
		configInput(RESET_CV_INPUT, "Reset");
		configInput(SIGNAL_INPUT, "Audio");

		configInput(BYPASS_CV_INPUT, "Bypass CV");
		//Outputs
		configOutput(SIGNAL_OUTPUT, "Audio");

	}

	void resetFades(){
		fade_in_fx = 0.0f;
		fade_in_dry = 0.0f;
		fade_out_fx = 1.0f;
		fade_out_dry = 1.0f;
	}

	void process(const ProcessArgs &args) override{

		if (bypass_button_trig.process(params[BYPASS_SWITCH].getValue()) || bypass_cv_trig.process(inputs[BYPASS_CV_INPUT].getVoltage()) ){
			fx_bypass = !fx_bypass;
			resetFades();
		}
		lights[BYPASS_LED].value = fx_bypass ? 1.0f : 0.0f;

		input_signal = clamp(inputs[SIGNAL_INPUT].getVoltage(),-10.0f,10.0f);

		//oscillator.setPitch(params[FREQ_PARAM].getValue());
		

		oscillator.setPitch( clamp(rescale(params[FREQ_PARAM].getValue(), 0.0f, 1.0f, 0.0f, 3.5f) + rescale(inputs[FREQ_CV_INPUT].getVoltage()/10, 0.0f, 1.0f, 0.0f, 3.5f),0.0f, 3.5f) );
		oscillator.offset = (1.0f);
		oscillator.invert = (params[INVERT_PARAM].getValue() <= 0.0f);
		oscillator.setPulseWidth(0.5f);
		oscillator.step(1.0f / args.sampleRate);
		oscillator.setReset(inputs[RESET_CV_INPUT].getVoltage());

		float wave = clamp( params[WAVE_PARAM].getValue() + inputs[WAVE_CV_INPUT].getVoltage(), 0.0f, 1.0f );
		float interp = crossfade(oscillator.sin(), oscillator.tri(), wave);

		lfo_modulation = 5.0f * interp;

		tremolo_signal = input_signal * clamp(lfo_modulation/10.0f, 0.0f, 1.0f);
		blend_control = clamp(params[BLEND_PARAM].getValue() + inputs[BLEND_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		output_signal = crossfade(input_signal,tremolo_signal,blend_control);

		//check bypass switch status
		if (fx_bypass){
			fade_in_dry += fade_speed;
			if ( fade_in_dry > 1.0f ) {
				fade_in_dry = 1.0f;
			}
			fade_out_fx -= fade_speed;
			if ( fade_out_fx < 0.0f ) {
				fade_out_fx = 0.0f;
			}
			outputs[SIGNAL_OUTPUT].setVoltage(( input_signal * fade_in_dry ) + ( output_signal * fade_out_fx ));
		}else{
			fade_in_fx += fade_speed;
			if ( fade_in_fx > 1.0f ) {
				fade_in_fx = 1.0f;
			}
			fade_out_dry -= fade_speed;
			if ( fade_out_dry < 0.0f ) {
				fade_out_dry = 0.0f;
			}
			outputs[SIGNAL_OUTPUT].setVoltage(( input_signal * fade_out_dry ) + ( output_signal * fade_in_fx ));
		}

		lights[PHASE_POS_LIGHT].setSmoothBrightness(fmaxf(0.0f, oscillator.light()), args.sampleTime);
		lights[PHASE_NEG_LIGHT].setSmoothBrightness(fmaxf(0.0f, -oscillator.light()), args.sampleTime);
		lights[BLEND_LIGHT].value = clamp(params[BLEND_PARAM].getValue() + inputs[BLEND_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);

	}


	json_t *dataToJson()override {
		json_t *rootJm = json_object();

		json_t *statesJ = json_array();
		
			json_t *bypassJ = json_boolean(fx_bypass);
			json_array_append_new(statesJ, bypassJ);
		
		json_object_set_new(rootJm, "as_FxBypass", statesJ);

		return rootJm;
	}

	void dataFromJson(json_t *rootJm)override {
		json_t *statesJ = json_object_get(rootJm, "as_FxBypass");
		
			json_t *bypassJ = json_array_get(statesJ, 0);

			fx_bypass = !!json_boolean_value(bypassJ);
		
	}
	
};


struct TremoloFxWidget : ModuleWidget { 

	TremoloFxWidget(TremoloFx *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Tremolo.svg")));
	
		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//phase switch
		addParam(createParam<as_CKSSwhite>(Vec(13, 100), module, TremoloFx::INVERT_PARAM));
		//KNOBS  
		addParam(createParam<as_FxKnobWhite>(Vec(43, 60), module, TremoloFx::WAVE_PARAM));
		addParam(createParam<as_FxKnobWhite>(Vec(43, 125), module, TremoloFx::FREQ_PARAM));
		addParam(createParam<as_FxKnobWhite>(Vec(43, 190), module, TremoloFx::BLEND_PARAM));
		//LIGHTS
		addChild(createLight<SmallLight<YellowRedLight>>(Vec(39, 122), module, TremoloFx::PHASE_POS_LIGHT));
		addChild(createLight<SmallLight<YellowLight>>(Vec(39, 187), module, TremoloFx::BLEND_LIGHT));
		//BYPASS SWITCH
/* 		addParam(createParam<LEDBezel>(Vec(55, 260), module, TremoloFx::BYPASS_SWITCH));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(57.2, 262), module, TremoloFx::BYPASS_LED)); */

		addParam(createParam<LEDBezel>(Vec(61, 260), module, TremoloFx::BYPASS_SWITCH ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(63.2, 262.2), module, TremoloFx::BYPASS_LED));

		//INS/OUTS
		addInput(createInput<as_PJ301MPort>(Vec(10, 310), module, TremoloFx::SIGNAL_INPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(55, 310), module, TremoloFx::SIGNAL_OUTPUT));
		//CV INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(10, 67), module, TremoloFx::WAVE_CV_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(10, 132), module, TremoloFx::FREQ_CV_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(10, 197), module, TremoloFx::BLEND_CV_INPUT));

		//BYPASS CV INPUT
		//addInput(createInput<as_PJ301MPort>(Vec(10, 259), module, TremoloFx::BYPASS_CV_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(33.5, 259), module, TremoloFx::BYPASS_CV_INPUT));
		//RESET CV
		addInput(createInput<as_PJ301MPort>(Vec(6, 259), module, TremoloFx::RESET_CV_INPUT));
	
	}
};

Model *modelTremoloFx = createModel<TremoloFx, TremoloFxWidget>("TremoloFx");