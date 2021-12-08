//**************************************************************************************
//Delay Plus module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code based on Fundamental plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"
/*
#include "dsp/samplerate.hpp"
#include "dsp/ringbuffer.hpp"
#include "dsp/filter.hpp"
#include "dsp/digital.hpp"
*/
#include <sstream>
#include <iomanip>

#define HISTORY_SIZE (1<<21)

struct DelayPlusFx : Module {
	enum ParamIds {
		TIME_PARAM,
		FEEDBACK_PARAM,
		COLOR_PARAM,
		MIX_PARAM,
		BYPASS_SWITCH,
		NUM_PARAMS
	};
	enum InputIds {
		TIME_INPUT,
		FEEDBACK_INPUT,
		COLOR_INPUT,
		COLOR_RETURN,

		MIX_INPUT,
		IN_INPUT,
		BYPASS_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		COLOR_SEND,
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	  enum LightIds {
		BYPASS_LED,
		NUM_LIGHTS
	}; 

	dsp::RCFilter lowpassFilter;
	dsp::RCFilter highpassFilter;

	dsp::DoubleRingBuffer<float, HISTORY_SIZE> historyBuffer;
	dsp::DoubleRingBuffer<float, 16> outBuffer;
	
	dsp::SampleRateConverter<1> src;

	dsp::SchmittTrigger bypass_button_trig;
	dsp::SchmittTrigger bypass_cv_trig;

	int lcd_tempo = 0;
	bool fx_bypass = false;
	float lastWet = 0.0f;

	float fade_in_fx = 0.0f;
	float fade_in_dry = 0.0f;
	float fade_out_fx = 1.0f;
	float fade_out_dry = 1.0f;
    const float fade_speed = 0.001f;

	void resetFades(){
		fade_in_fx = 0.0f;
		fade_in_dry = 0.0f;
		fade_out_fx = 1.0f;
		fade_out_dry = 1.0f;
	}

	DelayPlusFx() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(DelayPlusFx::TIME_PARAM, 0.0f, 10.0f, 0.350f, "Time", " MS", 0.0f, 1000.0f);
		configParam(DelayPlusFx::FEEDBACK_PARAM, 0.0f, 1.0f, 0.5f, "Feedback", "%", 0.0f, 100.0f);
		configParam(DelayPlusFx::COLOR_PARAM, 0.0f, 1.0f, 0.5f, "Color", "%", 0.0f, 100.0f);
		configParam(DelayPlusFx::MIX_PARAM, 0.0f, 1.0f, 0.5f, "Mix", "%", 0.0f, 100.0f);
		
		//New in V2, config temporary buttons info without displaying values
		configButton(BYPASS_SWITCH, "Bypass");

		//new V2, port labels
		//Inputs
		configInput(TIME_INPUT, "Time  CV");
		configInput(FEEDBACK_INPUT, "Feedback CV");
		configInput(COLOR_INPUT, "Color CV");
		configInput(COLOR_RETURN, "Color Return");
		configInput(IN_INPUT, "Audio");

		configInput(MIX_INPUT, "Mix CV");
		configInput(BYPASS_CV_INPUT, "Bypass CV");
		//Outputs
		configOutput(COLOR_SEND, "Color Send");
		configOutput(OUT_OUTPUT, "Audio");

	}


	void process(const ProcessArgs &args) override {

		if (bypass_button_trig.process(params[BYPASS_SWITCH].getValue()) || bypass_cv_trig.process(inputs[BYPASS_CV_INPUT].getVoltage()) ){
			fx_bypass = !fx_bypass;
			resetFades();
		}
		lights[BYPASS_LED].value = fx_bypass ? 1.0f : 0.0f;

		// Get input to delay block
		float signal_input = inputs[IN_INPUT].getVoltage();
		float feedback = clamp(params[FEEDBACK_PARAM].getValue() + inputs[FEEDBACK_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		float dry = signal_input + lastWet * feedback;

		// Compute delay time in seconds
		//float delay = 1e-3 * powf(10.0 / 1e-3, clampf(params[TIME_PARAM].getValue() + inputs[TIME_INPUT].getVoltage() / 10.0, 0.0, 1.0));
		float delay = clamp(params[TIME_PARAM].getValue() + inputs[TIME_INPUT].getVoltage(), 0.0f, 10.0f);
		//LCD display tempo  - show value as ms
		lcd_tempo = std::round(delay*1000);
		// Number of delay samples
		float index = delay * args.sampleRate;

		// Push dry sample into history buffer
		if (!historyBuffer.full()) {
			historyBuffer.push(dry);
		}

		// How many samples do we need consume to catch up?
		float consume = index - historyBuffer.size();

		if (outBuffer.empty()) {
			double ratio = 1.0;
			if (consume <= -16)
				ratio = 0.5;
			else if (consume >= 16)
				ratio = 2.0;

			float inSR = args.sampleRate;
			float outSR = ratio * inSR;

			int inFrames = fmin(historyBuffer.size(), 16);
			int outFrames = outBuffer.capacity();
			src.setRates(inSR, outSR);
			src.process((const dsp::Frame<1>*)historyBuffer.startData(), &inFrames, (dsp::Frame<1>*)outBuffer.endData(), &outFrames);
			historyBuffer.startIncr(inFrames);
			outBuffer.endIncr(outFrames);
		}

		float out;
		float mix;
		float wet = 0.0f;
		if (!outBuffer.empty()) {
			wet = outBuffer.shift();
		}

		if (outputs[COLOR_SEND].isConnected() == false) {
			//internal color
			// Apply color to delay wet output
			float color = clamp(params[COLOR_PARAM].getValue() + inputs[COLOR_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
			float lowpassFreq = 10000.0f * powf(10.0f, clamp(2.0*color, 0.0f, 1.0f));
			lowpassFilter.setCutoff(lowpassFreq / args.sampleRate);
			lowpassFilter.process(wet);
			wet = lowpassFilter.lowpass();
			float highpassFreq = 10.0f * powf(100.0f, clamp(2.0f*color - 1.0f, 0.0f, 1.0f));
			highpassFilter.setCutoff(highpassFreq / args.sampleRate);
			highpassFilter.process(wet);
			wet = highpassFilter.highpass();
			//lastWet = wet;
		}else {
		//external color, to filter the wet delay signal outside of the module, or to feed another module
			outputs[COLOR_SEND].setVoltage(wet);
			wet = inputs[COLOR_RETURN].getVoltage();	
		}
		lastWet = wet;
		mix = clamp(params[MIX_PARAM].getValue() + inputs[MIX_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		out = crossfade(signal_input, wet, mix);
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
			outputs[OUT_OUTPUT].setVoltage(( signal_input * fade_in_dry ) + ( out * fade_out_fx ));
		}else{
			fade_in_fx += fade_speed;
			if ( fade_in_fx > 1.0f ) {
				fade_in_fx = 1.0f;
			}
			fade_out_dry -= fade_speed;
			if ( fade_out_dry < 0.0f ) {
				fade_out_dry = 0.0f;
			}
			outputs[OUT_OUTPUT].setVoltage(( signal_input * fade_out_dry ) + ( out * fade_in_fx ));
		}
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

///////////////////////////////////
struct MsDisplayWidget : TransparentWidget {

	int *value = NULL;
	std::shared_ptr<Font> font;
	std::string fontPath = asset::plugin(pluginInstance, "res/Segment7Standard.ttf");

	void drawLayer(const DrawArgs& args, int layer) override {
		if (layer != 1){
			return;
		}
		if (!value) {
			return;
		}
		font = APP->window->loadFont(fontPath);
		// text 
		if (font) {
			nvgFontSize(args.vg, 18);
			nvgFontFaceId(args.vg, font->handle);
			nvgTextLetterSpacing(args.vg, 2.5);

			std::stringstream to_display;   
			to_display << std::right  << std::setw(5) << *value;
			//to_display << std::right << *value;
			Vec textPos = Vec(4.0f, 17.0f); 
			
			NVGcolor textColor = nvgRGB(0xf0, 0x00, 0x00);
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);

		}

	}
};
////////////////////////////////////
struct DelayPlusFxWidget : ModuleWidget { 

	DelayPlusFxWidget(DelayPlusFx *module) {
		
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/DelayPlus.svg")));

		//MS DISPLAY 
		MsDisplayWidget *display = new MsDisplayWidget();
		display->box.pos = Vec(14,50);
		display->box.size = Vec(70, 20);
		if (module) {
            display->value = &module->lcd_tempo;
        }
		addChild(display); 	
		int y_offset=40;
		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//KNOBS
		addParam(createParam<as_FxKnobWhite>(Vec(74, 38+y_offset), module, DelayPlusFx::TIME_PARAM));
		addParam(createParam<as_FxKnobWhite>(Vec(74, 90+y_offset), module, DelayPlusFx::FEEDBACK_PARAM));
		addParam(createParam<as_FxKnobWhite>(Vec(74, 140+y_offset), module, DelayPlusFx::COLOR_PARAM));
		addParam(createParam<as_FxKnobWhite>(Vec(74, 213+y_offset), module, DelayPlusFx::MIX_PARAM));
		//BYPASS SWITCH
		addParam(createParam<LEDBezel>(Vec(49.5, 250+y_offset), module, DelayPlusFx::BYPASS_SWITCH ));//Y=272
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(51.7, 252+y_offset), module, DelayPlusFx::BYPASS_LED));//Y=274
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(10, 45+y_offset), module, DelayPlusFx::TIME_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(10, 95+y_offset), module, DelayPlusFx::FEEDBACK_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(10, 145+y_offset), module, DelayPlusFx::COLOR_INPUT));
		//DELAY SIGNAL SEND
		addOutput(createOutput<as_PJ301MPortGold>(Vec(20, 184+y_offset), module, DelayPlusFx::COLOR_SEND));
		//DELAY SIGNAL RETURN
		addInput(createInput<as_PJ301MPort>(Vec(75, 184+y_offset), module, DelayPlusFx::COLOR_RETURN));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(10, 220+y_offset), module, DelayPlusFx::MIX_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(10, 310), module, DelayPlusFx::IN_INPUT));
		//OUTPUT
		addOutput(createOutput<as_PJ301MPortGold>(Vec(85, 310), module, DelayPlusFx::OUT_OUTPUT));

		//BYPASS CV INPUT
		addInput(createInput<as_PJ301MPort>(Vec(49, 320), module, DelayPlusFx::BYPASS_CV_INPUT));

	}
};


Model *modelDelayPlusFx = createModel<DelayPlusFx, DelayPlusFxWidget>("DelayPlusFx");