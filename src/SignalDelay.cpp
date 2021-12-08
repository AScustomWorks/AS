//**************************************************************************************
//Signal Delay module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"
/*
#include "dsp/samplerate.hpp"
#include "dsp/ringbuffer.hpp"
*/
#include <sstream>
#include <iomanip>

#define HISTORY_SIZE (1<<21)

struct SignalDelay : Module {
	enum ParamIds {
		TIME_1_PARAM,
		TIME_2_PARAM,
		SWITCH_1_MODE,
		SWITCH_2_MODE,
		NUM_PARAMS
	};
	enum InputIds {
		TIME_1_INPUT,
		TIME_2_INPUT,
		IN_1_INPUT,
		IN_2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SEND_1_OUTPUT,
		SEND_2_OUTPUT,
		OUT_1_OUTPUT,
		OUT_2_OUTPUT,
		NUM_OUTPUTS
	};

	dsp::DoubleRingBuffer<float, HISTORY_SIZE> historyBuffer1;
	dsp::DoubleRingBuffer<float, 16> outBuffer1;
	dsp::SampleRateConverter<1> src1;
	float lastWet1 = 0.0f;
	int lcd_tempo1 = 0;
	int snd_mode_1 = 0;

	dsp::DoubleRingBuffer<float, HISTORY_SIZE> historyBuffer2;
	dsp::DoubleRingBuffer<float, 16> outBuffer2;
	dsp::SampleRateConverter<1> src2;
	float lastWet2 = 0.0f;
	int lcd_tempo2 = 0;
	int snd_mode_2 = 0;


	SignalDelay() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(SignalDelay::TIME_1_PARAM, 0.0f, 10.0f, 0.350f, "CH 1 Delay", " MS", 0.0f, 1000.0f);
		configParam(SignalDelay::TIME_2_PARAM, 0.0f, 10.0f, 0.350f, "CH 2 Delay", " MS", 0.0f, 1000.0f);

		//New in V2, config switches info without displaying values
		configSwitch(SWITCH_1_MODE, 0.0f, 1.0f, 0.0f, "CH 2 send output mode", {"Post", "Pre"});
		configSwitch(SWITCH_2_MODE, 0.0f, 1.0f, 0.0f, "CH 2 send output mode", {"Post", "Pre"});
		
		//new V2, port labels
		//Inputs
		configInput(TIME_1_INPUT, "CH 1 Time CV");
		configInput(TIME_2_INPUT, "CH 2 Time CV");
		configInput(IN_1_INPUT, "CH 1");
		configInput(IN_2_INPUT, "CH 2");
		//Outputs
		configOutput(SEND_1_OUTPUT, "CH 1 Send");
		configOutput(SEND_2_OUTPUT, "CH 2 Send");
		configOutput(OUT_1_OUTPUT, "CH 1");	
		configOutput(OUT_2_OUTPUT, "CH 2");				

	}


	void process(const ProcessArgs &args) override {

		snd_mode_1 = params[SWITCH_1_MODE].getValue();

		// DELAY 1 Get input to delay block
		float in1 = inputs[IN_1_INPUT].getVoltage();
		float feedback1 = 0;//only one repetition, for regular use: clampf(params[FEEDBACK_PARAM].getValue() + inputs[FEEDBACK_INPUT].getVoltage() / 10.0, 0.0, 1.0);
		float dry1 = in1 + lastWet1 * feedback1;
		// Compute delay time in seconds
		//delay time in seconds. Linear reading, now easy to setup any value by the digit
		float delay1 = clamp(params[TIME_1_PARAM].getValue() + inputs[TIME_1_INPUT].getVoltage(), 0.0f, 10.0f);
		//LCD display tempo  - show value as ms
		lcd_tempo1 = std::round(delay1*1000);
		// Number of delay samples
		float index1 = delay1 * args.sampleRate;
		// Push dry1 sample into history buffer
		if (!historyBuffer1.full()) {
			historyBuffer1.push(dry1);
		}
		// How many samples do we need consume1 to catch up?
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

		float wet1 = 0.0f;
		if (!outBuffer1.empty()) {
			wet1 = outBuffer1.shift();
		}
		if(snd_mode_1){
			outputs[SEND_1_OUTPUT].setVoltage(in1);
		}else{
			outputs[SEND_1_OUTPUT].setVoltage(wet1);
		}
		outputs[OUT_1_OUTPUT].setVoltage(wet1);	
		lastWet1 = wet1;

		//DELAY 2
		snd_mode_2 = params[SWITCH_2_MODE].getValue();
		// DELAY 2 Get input to delay block
		float in2 = inputs[IN_2_INPUT].getVoltage();
		float feedback2 = 0;//only one repetition, for regular use: clamp(params[FEEDBACK_PARAM].getValue() + inputs[FEEDBACK_INPUT].getVoltage() / 10.0, 0.0, 1.0);
		float dry2 = in2 + lastWet2 * feedback2;
		// Compute delay time in seconds
		//delay time in seconds. Linear reading, now easy to setup any value by the digit
		float delay2 = clamp(params[TIME_2_PARAM].getValue() + inputs[TIME_2_INPUT].getVoltage(), 0.0f, 10.0f);
		//LCD display tempo  - show value as ms
		lcd_tempo2 = std::round(delay2*1000);
		// Number of delay samples
		float index2 = delay2 * args.sampleRate;
		// Push dry sample into history buffer
		if (!historyBuffer2.full()) {
			historyBuffer2.push(dry2);
		}
		// How many samples do we need consume1 to catch up?
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

		float wet2 = 0.0;
		if (!outBuffer2.empty()) {
			wet2 = outBuffer2.shift();
		}
		if(snd_mode_2){
			outputs[SEND_2_OUTPUT].setVoltage(in2);
		}else{
			outputs[SEND_2_OUTPUT].setVoltage(wet2);
		}
		outputs[OUT_2_OUTPUT].setVoltage(wet2);	
		lastWet2 = wet2;
	}

};

///////////////////////////////////
struct MsDelayDisplayWidget : TransparentWidget {

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

			Vec textPos = Vec(4.0f, 17.0f); 

			NVGcolor textColor = nvgRGB(0xf0, 0x00, 0x00);
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
		}
  }
};
////////////////////////////////////

struct SignalDelayWidget : ModuleWidget { 

   SignalDelayWidget(SignalDelay *module) {

		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SignalDelay.svg")));

		//DELAY 1
		//MS DISPLAY 
		
		MsDelayDisplayWidget *display1 = new MsDelayDisplayWidget();
		display1->box.pos = Vec(10,50);
		display1->box.size = Vec(70, 20);
		if (module) {
            display1->value = &module->lcd_tempo1;
        }
		addChild(display1); 
		
		static const float posX[3] = {3,33,63};	
		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//KNOBS
		addParam(createParam<as_KnobBlack>(Vec(47, 77), module, SignalDelay::TIME_1_PARAM));
		//CV INPUT
		addInput(createInput<as_PJ301MPort>(Vec(posX[0]+5, 84), module, SignalDelay::TIME_1_INPUT));
		//INPUT
		addInput(createInput<as_PJ301MPort>(Vec(posX[0], 166), module, SignalDelay::IN_1_INPUT));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(posX[1], 166), module, SignalDelay::SEND_1_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(posX[2], 166), module, SignalDelay::OUT_1_OUTPUT));

		//DELAY 2
		//MS DISPLAY 
		static const int mod_offset=160;
		
		MsDelayDisplayWidget *display2 = new MsDelayDisplayWidget();
		display2->box.pos = Vec(10,44+mod_offset);
		display2->box.size = Vec(70, 20);
		if (module) {
            display2->value = &module->lcd_tempo2;
        }
		addChild(display2); 
		
		//KNOBS
		addParam(createParam<as_KnobBlack>(Vec(47, 71+mod_offset), module, SignalDelay::TIME_2_PARAM));
		//CV INPUT
		addInput(createInput<as_PJ301MPort>(Vec(posX[0]+5, 78+mod_offset), module, SignalDelay::TIME_2_INPUT));
		//INPUT
		addInput(createInput<as_PJ301MPort>(Vec(posX[0], 159+mod_offset), module, SignalDelay::IN_2_INPUT));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(posX[1], 159+mod_offset), module, SignalDelay::SEND_2_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(posX[2], 159+mod_offset), module, SignalDelay::OUT_2_OUTPUT));

		//SEND MODE SWITCH THRU/DELAYED
		addParam(createParam<as_CKSSH>(Vec(33, 131), module, SignalDelay::SWITCH_1_MODE));
		addParam(createParam<as_CKSSH>(Vec(33, 125+mod_offset), module, SignalDelay::SWITCH_2_MODE));

	}
};


Model *modelSignalDelay = createModel<SignalDelay, SignalDelayWidget>("SignalDelay");