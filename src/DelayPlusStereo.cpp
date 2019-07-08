//**************************************************************************************
//Delay Plus module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
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

struct DelayPlusStereoFx : Module {
	enum ParamIds {
		TIME_PARAM_1,
		FEEDBACK_PARAM_1,
		COLOR_PARAM_1,

		TIME_PARAM_2,
		FEEDBACK_PARAM_2,
		COLOR_PARAM_2,
		FBK_LINK_PARAM,
		COLOR_LINK_PARAM,

		MIX_PARAM,
		BYPASS_SWITCH,
		NUM_PARAMS
	};
	enum InputIds {
		TIME_CV_INPUT_1,
		FEEDBACK__CV_INPUT_1,
		COLOR_CV_INPUT_1,
		COLOR_RETURN_1,

		TIME_CV_INPUT_2,
		FEEDBACK__CV_INPUT_2,
		COLOR_CV_INPUT_2,
		COLOR_RETURN_2,

		MIX_CV_INPUT,
		SIGNAL_INPUT_1,
		SIGNAL_INPUT_2,
		BYPASS_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		COLOR_SEND_1,
		COLOR_SEND_2,

		SIGNAL_OUTPUT_1,
		SIGNAL_OUTPUT_2,
		NUM_OUTPUTS
	};
	  enum LightIds {
		BYPASS_LED,
		NUM_LIGHTS
	}; 

	dsp::RCFilter lowpassFilter1;
	dsp::RCFilter highpassFilter1;

	dsp::RCFilter lowpassFilter2;
	dsp::RCFilter highpassFilter2;

	dsp::DoubleRingBuffer<float, HISTORY_SIZE> historyBuffer1;
	dsp::DoubleRingBuffer<float, 16> outBuffer1;

	dsp::DoubleRingBuffer<float, HISTORY_SIZE> historyBuffer2;
	dsp::DoubleRingBuffer<float, 16> outBuffer2;
	
	dsp::SampleRateConverter<1> src1;
	dsp::SampleRateConverter<1> src2;

	dsp::SchmittTrigger bypass_button_trig;
	dsp::SchmittTrigger bypass_cv_trig;

	int lcd_tempo1 = 0;
	int lcd_tempo2 = 0;
	bool fx_bypass = false;
	float lastWet1 = 0.0f;
	float lastWet2 = 0.0f;
	float feedback1 = 0.0f;
	float feedback2 = 0.0f;
	float color1 = 0.0f;
	float color2 = 0.0f;
	float mix_value = 0.0f;

	float fade_in_fx = 0.0f;
	float fade_in_dry = 0.0f;
	float fade_out_fx = 1.0f;
	float fade_out_dry = 1.0f;
    const float fade_speed = 0.001f;

	float signal_input_1 = 0.0f;
	float signal_input_2 = 0.0f;

	void resetFades(){
		fade_in_fx = 0.0f;
		fade_in_dry = 0.0f;
		fade_out_fx = 1.0f;
		fade_out_dry = 1.0f;
	}

	DelayPlusStereoFx() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);


		configParam(DelayPlusStereoFx::TIME_PARAM_1, 0.0f, 10.0f, 0.375f, "Time L", " MS", 0.0f, 1000.0f);
		configParam(DelayPlusStereoFx::FEEDBACK_PARAM_1, 0.0f, 1.0f, 0.5f, "Feedback L", "%", 0.0f, 100.0f);
		configParam(DelayPlusStereoFx::COLOR_PARAM_1, 0.0f, 1.0f, 0.5f, "Color L", "%", 0.0f, 100.0f);
		configParam(DelayPlusStereoFx::TIME_PARAM_2, 0.0f, 10.0f, 0.750f, "Time R", " MS", 0.0f, 1000.0f);
		configParam(DelayPlusStereoFx::FEEDBACK_PARAM_2, 0.0f, 1.0f, 0.5f, "Feedback R", "%", 0.0f, 100.0f);
		configParam(DelayPlusStereoFx::COLOR_PARAM_2, 0.0f, 1.0f, 0.5f, "Color R", "%", 0.0f, 100.0f);
		configParam(DelayPlusStereoFx::FBK_LINK_PARAM, 0.0f, 1.0f, 0.0f, "Link channels");
		configParam(DelayPlusStereoFx::COLOR_LINK_PARAM, 0.0f, 1.0f, 0.0f, "Link Color");
		configParam(DelayPlusStereoFx::MIX_PARAM, 0.0f, 1.0f, 0.5f, "Mix", "%", 0.0f, 100.0f);
		configParam(DelayPlusStereoFx::BYPASS_SWITCH , 0.0f, 1.0f, 0.0f, "Bypass");

	}

	void process(const ProcessArgs &args) override{

		if (bypass_button_trig.process(params[BYPASS_SWITCH].getValue()) || bypass_cv_trig.process(inputs[BYPASS_CV_INPUT].getVoltage()) ){
			fx_bypass = !fx_bypass;
			resetFades();
		}
		lights[BYPASS_LED].value = fx_bypass ? 1.0f : 0.0f;

		signal_input_1 = inputs[SIGNAL_INPUT_1].getVoltage();
		//check for MONO/STEREO CONNECTIONS
		if(!inputs[SIGNAL_INPUT_2].isConnected()){
			signal_input_2 = inputs[SIGNAL_INPUT_1].getVoltage();
		}else{
			signal_input_2 = inputs[SIGNAL_INPUT_2].getVoltage();
		}
			
		// DELAY L - Feed input to delay block
		feedback1 = clamp(params[FEEDBACK_PARAM_1].getValue() + inputs[FEEDBACK__CV_INPUT_1].getVoltage() / 10.0f, 0.0f, 1.0f);
		float dry1 = signal_input_1 + lastWet1 * feedback1;

		// Compute delay time in seconds
		//float delay = 1e-3 * powf(10.0 / 1e-3, clampf(params[TIME_PARAM_1].getValue() + inputs[TIME_CV_INPUT_1].getVoltage() / 10.0, 0.0, 1.0));
		float delay1 = clamp(params[TIME_PARAM_1].getValue() + inputs[TIME_CV_INPUT_1].getVoltage(), 0.0f, 10.0f);
		//LCD display tempo  - show value as ms
		lcd_tempo1 = std::round(delay1*1000);
		// Number of delay samples
		float index1 = delay1 * args.sampleRate;

		// Push dry sample into history buffer
		if (!historyBuffer1.full()) {
			historyBuffer1.push(dry1);
		}

		// How many samples do we need consume to catch up?
		float consume1 = index1 - historyBuffer1.size();

		if (outBuffer1.empty()) {
			double ratio1 = 1.0;
			if (consume1 <= -16)
				ratio1 = 0.5;
			else if (consume1 >= 16)
				ratio1 = 2.0;

			float inSR1 = args.sampleRate;
			float outSR1 = ratio1 * inSR1;

			int inFrames1 = fmin(historyBuffer1.size(), 16);
			int outFrames1 = outBuffer1.capacity();
			src1.setRates(inSR1, outSR1);
			src1.process((const dsp::Frame<1>*)historyBuffer1.startData(), &inFrames1, (dsp::Frame<1>*)outBuffer1.endData(), &outFrames1);
			historyBuffer1.startIncr(inFrames1);
			outBuffer1.endIncr(outFrames1);
		}

		float out1;
		float wet1 = 0.0f;
		if (!outBuffer1.empty()) {
			wet1 = outBuffer1.shift();
		}

		if (!outputs[COLOR_SEND_1].isConnected()) {
			//internal color
			// Apply color to delay wet output
			color1 = clamp(params[COLOR_PARAM_1].getValue() + inputs[COLOR_CV_INPUT_1].getVoltage() / 10.0f, 0.0f, 1.0f);
			float lowpassFreq1 = 10000.0f * powf(10.0f, clamp(2.0*color1, 0.0f, 1.0f));
			lowpassFilter1.setCutoff(lowpassFreq1 / args.sampleRate);
			lowpassFilter1.process(wet1);
			wet1 = lowpassFilter1.lowpass();
			float highpassFreq1 = 10.0f * powf(100.0f, clamp(2.0f*color1 - 1.0f, 0.0f, 1.0f));
			highpassFilter1.setCutoff(highpassFreq1 / args.sampleRate);
			highpassFilter1.process(wet1);
			wet1 = highpassFilter1.highpass();
		}else {
		//external color, to filter the wet delay signal outside of the module, or to feed another module
			outputs[COLOR_SEND_1].setVoltage(wet1);
			wet1 = inputs[COLOR_RETURN_1].getVoltage();	
		}
		lastWet1 = wet1;
		// end of delay 1 block

		// DELAY R - Feed input to delay block
		//CHECK IF LINK IS ENABLED FOR FEEDBACK KNOBS
		if(params[FBK_LINK_PARAM].getValue()){
				feedback2 = feedback1; 
		}else{
				feedback2 = clamp(params[FEEDBACK_PARAM_2].getValue() + inputs[FEEDBACK__CV_INPUT_2].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		float dry2 = signal_input_2 + lastWet2 * feedback2;

		// Compute delay time in seconds
		//float delay = 1e-3 * powf(10.0 / 1e-3, clampf(params[TIME_PARAM_1].getValue() + inputs[TIME_CV_INPUT_1].getVoltage() / 10.0, 0.0, 1.0));
		float delay2 = clamp(params[TIME_PARAM_2].getValue() + inputs[TIME_CV_INPUT_2].getVoltage(), 0.0f, 10.0f);
		//LCD display tempo  - show value as ms
		lcd_tempo2 = std::round(delay2*1000);
		// Number of delay samples
		float index2 = delay2 * args.sampleRate;

		// Push dry sample into history buffer
		if (!historyBuffer2.full()) {
			historyBuffer2.push(dry2);
		}

		// How many samples do we need consume to catch up?
		float consume2 = index2 - historyBuffer2.size();

		if (outBuffer2.empty()) {
			double ratio2 = 1.0;
			if (consume2 <= -16)
				ratio2 = 0.5;
			else if (consume2 >= 16)
				ratio2 = 2.0;

			float inSR2 = args.sampleRate;
			float outSR2 = ratio2 * inSR2;

			int inFrames2 = fmin(historyBuffer2.size(), 16);
			int outFrames2 = outBuffer2.capacity();
			src2.setRates(inSR2, outSR2);
			src2.process((const dsp::Frame<1>*)historyBuffer2.startData(), &inFrames2, (dsp::Frame<1>*)outBuffer2.endData(), &outFrames2);
			historyBuffer2.startIncr(inFrames2);
			outBuffer2.endIncr(outFrames2);
		}

		float out2;
		float wet2 = 0.0f;
		if (!outBuffer2.empty()) {
			wet2 = outBuffer2.shift();
		}

		if (!outputs[COLOR_SEND_2].isConnected()) {
			//internal color
			// Apply color to delay wet output
			if ( params[COLOR_LINK_PARAM].getValue() ) {
				color2 = color1;
			} else {
				color2 = clamp(params[COLOR_PARAM_2].getValue() + inputs[COLOR_CV_INPUT_2].getVoltage() / 10.0f, 0.0f, 1.0f);
			}
			float lowpassFreq2 = 10000.0f * powf(10.0f, clamp(2.0*color2, 0.0f, 1.0f));
			lowpassFilter2.setCutoff(lowpassFreq2 / args.sampleRate);
			lowpassFilter2.process(wet2);
			wet2 = lowpassFilter2.lowpass();
			float highpassFreq2 = 10.0f * powf(100.0f, clamp(2.0f*color2 - 1.0f, 0.0f, 1.0f));
			highpassFilter2.setCutoff(highpassFreq2 / args.sampleRate);
			highpassFilter2.process(wet2);
			wet2 = highpassFilter2.highpass();
		}else {
		//external color, to filter the wet delay signal outside of the module, or to feed another module
			outputs[COLOR_SEND_2].setVoltage(wet2);
			wet2 = inputs[COLOR_RETURN_2].getVoltage();	
		}
		lastWet2 = wet2;
		//mix dry & wet signals
		mix_value = clamp(params[MIX_PARAM].getValue() + inputs[MIX_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		out1 = crossfade(signal_input_1, wet1, mix_value);
		out2 = crossfade(signal_input_2, wet2, mix_value);
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
			outputs[SIGNAL_OUTPUT_1].setVoltage(( signal_input_1 * fade_in_dry ) + ( out1 * fade_out_fx ));
			outputs[SIGNAL_OUTPUT_2].setVoltage(( signal_input_2 * fade_in_dry ) + ( out2 * fade_out_fx ));
		}else{
			fade_in_fx += fade_speed;
			if ( fade_in_fx > 1.0f ) {
				fade_in_fx = 1.0f;
			}
			fade_out_dry -= fade_speed;
			if ( fade_out_dry < 0.0f ) {
				fade_out_dry = 0.0f;
			}
			outputs[SIGNAL_OUTPUT_1].setVoltage(( signal_input_1 * fade_out_dry ) + ( out1 * fade_in_fx ));
			outputs[SIGNAL_OUTPUT_2].setVoltage(( signal_input_2 * fade_out_dry ) + ( out2 * fade_in_fx ));
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

  MsDisplayWidget() {
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
  };

  void draw(const DrawArgs &args) override{
	if (!value) {
      return;
    }
    /*// Background
	NVGcolor backgroundColor = nvgRGB(0x20, 0x10, 0x10);
    NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(args.vg, backgroundColor);
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.5);
    nvgStrokeColor(args.vg, borderColor);
    nvgStroke(args.vg); 
	*/   
    // text 
    nvgFontSize(args.vg, 18);
    nvgFontFaceId(args.vg, font->handle);
    nvgTextLetterSpacing(args.vg, 2.5);

    std::stringstream to_display;   
    to_display << std::right  << std::setw(5) << *value;
	//to_display << std::right << *value;

    Vec textPos = Vec(4.0f, 17.0f); 
	/*
    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "~~~~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "\\\\\\\\\\", NULL);
	*/
    NVGcolor textColor = nvgRGB(0xf0, 0x00, 0x00);
    nvgFillColor(args.vg, textColor);
    nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
  }
};
////////////////////////////////////
struct DelayPlusStereoFxWidget : ModuleWidget { 

	DelayPlusStereoFxWidget(DelayPlusStereoFx *module) {
		
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/DelayPlusStereo.svg")));

		//MS DISPLAY L
		MsDisplayWidget *display1 = new MsDisplayWidget();
		display1->box.pos = Vec(7,50);
		display1->box.size = Vec(70, 20);
		if (module) {
            display1->value = &module->lcd_tempo1;
        }
		addChild(display1); 	

		//MS DISPLAY R
		MsDisplayWidget *display2 = new MsDisplayWidget();
		display2->box.pos = Vec(102,50);
		display2->box.size = Vec(70, 20);
		if (module) {
            display2->value = &module->lcd_tempo2;
        }
		addChild(display2); 
		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//KNOBS L
		addParam(createParam<as_FxKnobWhite>(Vec(37, 78), module, DelayPlusStereoFx::TIME_PARAM_1));
		addParam(createParam<as_FxKnobWhite>(Vec(37, 130), module, DelayPlusStereoFx::FEEDBACK_PARAM_1));
		addParam(createParam<as_FxKnobWhite>(Vec(37, 180), module, DelayPlusStereoFx::COLOR_PARAM_1));
		//KNOBS R
		addParam(createParam<as_FxKnobWhite>(Vec(106, 78), module, DelayPlusStereoFx::TIME_PARAM_2));
		addParam(createParam<as_FxKnobWhite>(Vec(106, 130), module, DelayPlusStereoFx::FEEDBACK_PARAM_2));
		addParam(createParam<as_FxKnobWhite>(Vec(106, 180), module, DelayPlusStereoFx::COLOR_PARAM_2));
		//FEEDBACK LINK SWITCH
		addParam(createParam<as_CKSSwhite>(Vec(82, 145), module, DelayPlusStereoFx::FBK_LINK_PARAM));
		//COLOR LINK SWITCH
		addParam(createParam<as_CKSSwhite>(Vec(82, 195), module, DelayPlusStereoFx::COLOR_LINK_PARAM));
		//MIX KNOB
		addParam(createParam<as_FxKnobWhite>(Vec(71, 251), module, DelayPlusStereoFx::MIX_PARAM));
		//BYPASS SWITCH
		addParam(createParam<LEDBezel>(Vec(79, 292), module, DelayPlusStereoFx::BYPASS_SWITCH ));
		addChild(createLight<LedLight<RedLight>>(Vec(79+2.2, 294), module, DelayPlusStereoFx::BYPASS_LED));
		//INPUTS CV L
		addInput(createInput<as_PJ301MPort>(Vec(7, 87), module, DelayPlusStereoFx::TIME_CV_INPUT_1));
		addInput(createInput<as_PJ301MPort>(Vec(7, 137), module, DelayPlusStereoFx::FEEDBACK__CV_INPUT_1));
		addInput(createInput<as_PJ301MPort>(Vec(7, 187), module, DelayPlusStereoFx::COLOR_CV_INPUT_1));
		//INPUTS CV R
		addInput(createInput<as_PJ301MPort>(Vec(150, 87), module, DelayPlusStereoFx::TIME_CV_INPUT_2));
		addInput(createInput<as_PJ301MPort>(Vec(150, 137), module, DelayPlusStereoFx::FEEDBACK__CV_INPUT_2));
		addInput(createInput<as_PJ301MPort>(Vec(150, 187), module, DelayPlusStereoFx::COLOR_CV_INPUT_2));

		//DELAY SIGNAL SEND L
		addOutput(createOutput<as_PJ301MPortGold>(Vec(15, 224), module, DelayPlusStereoFx::COLOR_SEND_1));
		//DELAY SIGNAL RETURN L
		addInput(createInput<as_PJ301MPort>(Vec(50, 224), module, DelayPlusStereoFx::COLOR_RETURN_1));

		//DELAY SIGNAL SEND R
		addOutput(createOutput<as_PJ301MPortGold>(Vec(105, 224), module, DelayPlusStereoFx::COLOR_SEND_2));
		//DELAY SIGNAL RETURN R
		addInput(createInput<as_PJ301MPort>(Vec(140, 224), module, DelayPlusStereoFx::COLOR_RETURN_2));

		//MIX CV INPUT
		addInput(createInput<as_PJ301MPort>(Vec(40, 258), module, DelayPlusStereoFx::MIX_CV_INPUT));
		//SIGNAL INPUT L
		addInput(createInput<as_PJ301MPort>(Vec(20, 300), module, DelayPlusStereoFx::SIGNAL_INPUT_1));
		//SIGNAL INPUT R
		addInput(createInput<as_PJ301MPort>(Vec(20, 330), module, DelayPlusStereoFx::SIGNAL_INPUT_2));
		//SIGNAL OUTPUT L
		addOutput(createOutput<as_PJ301MPortGold>(Vec(135, 300), module, DelayPlusStereoFx::SIGNAL_OUTPUT_1));
		//SIGNAL OUTPUT R
		addOutput(createOutput<as_PJ301MPortGold>(Vec(135, 330), module, DelayPlusStereoFx::SIGNAL_OUTPUT_2));

		//BYPASS CV INPUT
		addInput(createInput<as_PJ301MPort>(Vec(78, 322), module, DelayPlusStereoFx::BYPASS_CV_INPUT));

	}
};


Model *modelDelayPlusStereoFx = createModel<DelayPlusStereoFx, DelayPlusStereoFxWidget>( "DelayPlusStereoFx");