//**************************************************************************************
//Steps module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code taken from Dual Counter - VCV Module, Strum 2017
//**************************************************************************************

#include "AS.hpp"

//#include "dsp/digital.hpp"

#include <sstream>
#include <iomanip>

struct Steps : Module {
	enum ParamIds {
    RST_BUTTON1,
    COUNT_NUM_PARAM_1,
    RST_BUTTON2,
    COUNT_NUM_PARAM_2,
    RST_BUTTON3,
    COUNT_NUM_PARAM_3,
		NUM_PARAMS
	};  
	enum InputIds {
    CLK_IN_1,
    RESET_IN_1,
    CLK_IN_2,
    RESET_IN_2,
    CLK_IN_3,
    RESET_IN_3,	  
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
    OUTPUT_2,
    OUTPUT_3,    
		NUM_OUTPUTS
	};
    enum LightIds {
		RESET_LIGHT1,
		RESET_LIGHT2,
  	RESET_LIGHT3,
		NUM_LIGHTS
	};

  dsp::SchmittTrigger clock_trigger_1;
  dsp::SchmittTrigger reset_trigger_1;
  dsp::SchmittTrigger reset_ext_trigger_1;
  int count_limit1 = 1;
  int count1 = 0;
  dsp::SchmittTrigger clock_trigger_2;
  dsp::SchmittTrigger reset_trigger_2;
  dsp::SchmittTrigger reset_ext_trigger_2;
  int count_limit_2 = 1;
  int count_2 = 0;
  dsp::SchmittTrigger clock_trigger_3;
  dsp::SchmittTrigger reset_trigger_3;
  dsp::SchmittTrigger reset_ext_trigger_3;
  int count_limit_3 = 1;
  int count_3 = 0;
  const float lightLambda = 0.075f;
  float resetLight1 = 0.0f;
  float resetLight2 = 0.0f;
  float resetLight3 = 0.0f;

  dsp::PulseGenerator clockPulse1;
  bool pulse1 = false;
  dsp::PulseGenerator clockPulse2;
  bool pulse2 = false;
  dsp::PulseGenerator clockPulse3;
  bool pulse3 = false;

  Steps() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(Steps::COUNT_NUM_PARAM_1, 1.0f, 64.0f, 1.0f, "CH 1 Count");
    configParam(Steps::COUNT_NUM_PARAM_2, 1.0f, 64.0f, 1.0f, "CH 2 Count");
    configParam(Steps::COUNT_NUM_PARAM_3, 1.0f, 64.0f, 1.0f, "CH 3 Count");

      //New in V2, config switches info without displaying values
      configButton(RST_BUTTON1, "CH 1 Reset");
      configButton(RST_BUTTON2, "CH 2 Reset");
      configButton(RST_BUTTON3, "CH 3 Reset");
      //new V2, port labels

      //Inputs
      configInput(CLK_IN_1, "CH 1 Clock");
      configInput(CLK_IN_2, "CH 2 Clock");
      configInput(CLK_IN_3, "CH 3 Clock");
      configInput(RESET_IN_1, "CH 1 Reset");
      configInput(RESET_IN_2, "CH 2 Reset");
      configInput(RESET_IN_3, "CH 3 Reset");
      //Outputs
      configOutput(OUTPUT_1, "CH 1");
      configOutput(OUTPUT_2, "CH 2");
      configOutput(OUTPUT_3, "CH 3");

  }

  void process(const ProcessArgs &args) override {

    count_limit1 = round(params[COUNT_NUM_PARAM_1].getValue());
    count_limit_2 = round(params[COUNT_NUM_PARAM_2].getValue());
    count_limit_3 = round(params[COUNT_NUM_PARAM_3].getValue());
    
    bool reset1 = false;
    bool reset_2 = false;
    bool reset_3 = false;
    pulse1 = false;
    
      if (reset_trigger_1.process(params[RST_BUTTON1].getValue())){
          reset1 = true;
          count1 = 0;
          outputs[OUTPUT_1].setVoltage(0); 
          resetLight1 = 1.0f;

      }
      if (reset_ext_trigger_1.process(inputs[RESET_IN_1].getVoltage())){
          reset1 = true;
          count1 = 0;
          outputs[OUTPUT_1].setVoltage(0); 
          resetLight1 = 1.0f;

      } 

    resetLight1 -= resetLight1 / lightLambda / args.sampleRate;
    lights[RESET_LIGHT1].value = resetLight1;

    if (reset1 == false){
      if (clock_trigger_1.process(inputs[CLK_IN_1].getVoltage()) && count1 <= count_limit1)
            count1++;	
    }
    if (count1 == count_limit1){
        clockPulse1.trigger(1e-3);
    }
    if (count1 > count_limit1){
      count1 = 0;
    }
    pulse1 = clockPulse1.process(1.0 / args.sampleRate);
    outputs[OUTPUT_1].setVoltage(pulse1 ? 10.0f : 0.0f);
      
    ///////////// counter 2
    if (reset_trigger_2.process(params[RST_BUTTON2].getValue())){
      reset_2 = true;
      count_2 = 0;
      outputs[OUTPUT_2].setVoltage(0);
      resetLight2 = 1.0f;
    }
    if (reset_ext_trigger_2.process(inputs[RESET_IN_2].getVoltage())){
      reset_2 = true;
      count_2 = 0;
      outputs[OUTPUT_2].setVoltage(0);
      resetLight2 = 1.0f;
    } 
    resetLight2 -= resetLight2 / lightLambda / args.sampleRate;
    lights[RESET_LIGHT2].value = resetLight2;

    if (reset_2 == false){
      if (clock_trigger_2.process(inputs[CLK_IN_2].getVoltage()) && count_2 <= count_limit_2)
            count_2++;	
    }
    if (count_2 == count_limit_2){
      clockPulse2.trigger(1e-3);
    }
    if (count_2 > count_limit_2){
      count_2 = 0;
    }  
    pulse2 = clockPulse2.process(1.0 / args.sampleRate);
    outputs[OUTPUT_2].setVoltage(pulse2 ? 10.0f : 0.0f);
    ///////////// counter 3
    if (reset_trigger_3.process(params[RST_BUTTON3].getValue())){
      reset_3 = true;
      count_3 = 0;
      outputs[OUTPUT_3].setVoltage(0);
      resetLight3 = 1.0f;
    }
    if (reset_ext_trigger_3.process(inputs[RESET_IN_3].getVoltage())){
      reset_3 = true;
      count_3 = 0;
      outputs[OUTPUT_3].setVoltage(0);
      resetLight3 = 1.0f;
    }  
    resetLight3 -= resetLight3 / lightLambda / args.sampleRate;
    lights[RESET_LIGHT3].value = resetLight3;

    if (reset_3 == false){
      if (clock_trigger_3.process(inputs[CLK_IN_3].getVoltage()) && count_3 <= count_limit_3)
            count_3++;	
    }
    if (count_3 == count_limit_3){
      clockPulse3.trigger(1e-3);
    }
    if (count_3 > count_limit_3){
      count_3 = 0;
    }  
    pulse3 = clockPulse3.process(1.0 / args.sampleRate);
    outputs[OUTPUT_3].setVoltage(pulse3 ? 10.0f : 0.0f);
  }

};


///////////////////////////////////
struct NumberDisplayWidget : TransparentWidget {

  int *value = NULL;
  std::shared_ptr<Font> font;
  std::string fontPath = asset::plugin(pluginInstance, "res/Segment7Standard.ttf");

  void draw(const DrawArgs &args) override {
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
      to_display << std::right  << std::setw(2) << *value;

      Vec textPos = Vec(4.0f, 17.0f); 

      NVGcolor textColor = nvgRGB(0xf0, 0x00, 0x00);
      nvgFillColor(args.vg, textColor);
      nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
    }
  }
};
////////////////////////////////////

struct StepsWidget : ModuleWidget { 

  StepsWidget(Steps *module) {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Steps.svg"))); 

    //SCREWS
    addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    // counter 1
    //COUNT DISPLAY
      NumberDisplayWidget *display1 = new NumberDisplayWidget();
      display1->box.pos = Vec(9,50);
      display1->box.size = Vec(30, 20);
      if (module) {
        display1->value = &module->count1;
      }
      addChild(display1);
    //STEPS DISPLAY  
      NumberDisplayWidget *display2 = new NumberDisplayWidget();
      display2->box.pos = Vec(49,50);
      display2->box.size = Vec(30, 20);
      if (module) {
        display2->value = &module->count_limit1;
      }
      addChild(display2);

    int group_offset = 100;

      addParam(createParam<LEDBezel>(Vec(5, 82), module, Steps::RST_BUTTON1 ));
      addChild(createLight<LEDBezelLight<RedLight>>(Vec(5+2.2, 82+2.3), module, Steps::RESET_LIGHT1));

      addParam(createParam<as_KnobBlackSnap>(Vec(43, 73), module, Steps::COUNT_NUM_PARAM_1)); 

      addInput(createInput<as_PJ301MPort>(Vec(3, 120), module, Steps::RESET_IN_1));
      addInput(createInput<as_PJ301MPort>(Vec(33, 120), module, Steps::CLK_IN_1));
      addOutput(createOutput<as_PJ301MPortGold>(Vec(63, 120), module, Steps::OUTPUT_1));

    // counter 2
    //COUNT DISPLAY
      NumberDisplayWidget *display3 = new NumberDisplayWidget();
      display3->box.pos = Vec(9,50 + group_offset);
      display3->box.size = Vec(30, 20);
      if (module) {
        display3->value = &module->count_2;
      }
      addChild(display3);
    //STEPS DISPLAY  
      NumberDisplayWidget *display4 = new NumberDisplayWidget();
      display4->box.pos = Vec(49,50 + group_offset);
      display4->box.size = Vec(30, 20);
      if(module){
        display4->value = &module->count_limit_2;
      }
      addChild(display4);

      addParam(createParam<LEDBezel>(Vec(5, 82+ group_offset), module, Steps::RST_BUTTON2 ));
      addChild(createLight<LEDBezelLight<RedLight>>(Vec(5+2.2, 82+2.3+ group_offset), module, Steps::RESET_LIGHT2));

      addParam(createParam<as_KnobBlackSnap>(Vec(43, 73 + group_offset), module, Steps::COUNT_NUM_PARAM_2)); 

      addInput(createInput<as_PJ301MPort>(Vec(3, 120 + group_offset), module, Steps::RESET_IN_2));
      addInput(createInput<as_PJ301MPort>(Vec(33, 120 + group_offset), module, Steps::CLK_IN_2));
      addOutput(createOutput<as_PJ301MPortGold>(Vec(63, 120 + group_offset), module, Steps::OUTPUT_2));

    // counter 3
    //COUNT DISPLAY
      NumberDisplayWidget *display5 = new NumberDisplayWidget();
      display5->box.pos = Vec(9,50 + group_offset*2);
      display5->box.size = Vec(30, 20);
      if(module){
        display5->value = &module->count_3;
      }
      addChild(display5);
    //STEPS DISPLAY  
      NumberDisplayWidget *display6 = new NumberDisplayWidget();
      display6->box.pos = Vec(49,50 + group_offset*2);
      display6->box.size = Vec(30, 20);
      if(module){
        display6->value = &module->count_limit_3;
      }
      addChild(display6);

      addParam(createParam<LEDBezel>(Vec(5, 82+ group_offset*2), module, Steps::RST_BUTTON3 ));
      addChild(createLight<LEDBezelLight<RedLight>>(Vec(5+2.2, 82+2.3+ group_offset*2), module, Steps::RESET_LIGHT3));

      addParam(createParam<as_KnobBlackSnap>(Vec(43, 73 + group_offset*2), module, Steps::COUNT_NUM_PARAM_3)); 

      addInput(createInput<as_PJ301MPort>(Vec(3, 120 + group_offset*2), module, Steps::RESET_IN_3));
      addInput(createInput<as_PJ301MPort>(Vec(33, 120 + group_offset*2), module, Steps::CLK_IN_3));
      addOutput(createOutput<as_PJ301MPortGold>(Vec(63, 120 + group_offset*2), module, Steps::OUTPUT_3));	  
  }
};

Model *modelSteps = createModel<Steps, StepsWidget>("Steps");