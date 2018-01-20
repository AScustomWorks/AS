//**************************************************************************************
//Steps module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code taken from Dual Counter - VCV Module, Strum 2017
//**************************************************************************************

#include "AS.hpp"

#include "dsp/digital.hpp"

#include <sstream>
#include <iomanip>

struct Steps : Module {
	enum ParamIds {
    RST_BUTTON1,
    COUNT_NUM_PARAM,
    RST_BUTTON2,
    COUNT_NUM_PARAM_2,
    RST_BUTTON3,
    COUNT_NUM_PARAM_3,
		NUM_PARAMS
	};  
	enum InputIds {
    CLK_IN,
    RESET_IN,
    CLK_IN_2,
    RESET_IN_2,
    CLK_IN_3,
    RESET_IN_3,	  
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT,
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

    SchmittTrigger clock_trigger;
    SchmittTrigger reset_trigger;
    int count_limit = 0;
    int count = 0;
    SchmittTrigger clock_trigger_2;
    SchmittTrigger reset_trigger_2;
    int count_limit_2 = 0;
    int count_2 = 0;
    SchmittTrigger clock_trigger_3;
    SchmittTrigger reset_trigger_3;
    int count_limit_3 = 0;
    int count_3 = 0;
    const float lightLambda = 0.075;
    float resetLight1 = 0.0;
    float resetLight2 = 0.0;
    float resetLight3 = 0.0;
  

    Steps() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
      /*
        params.resize(NUM_PARAMS);
        inputs.resize(NUM_INPUTS);
        outputs.resize(NUM_OUTPUTS);
        clock_trigger.setThresholds(0.0, 1.0);
        reset_trigger.setThresholds(0.0, 1.0);
        clock_trigger_2.setThresholds(0.0, 1.0);
        reset_trigger_2.setThresholds(0.0, 1.0);
        */
    }

	void step() override;
};


void Steps::step(){

  count_limit = round(params[COUNT_NUM_PARAM].value);
  count_limit_2 = round(params[COUNT_NUM_PARAM_2].value);
  count_limit_3 = round(params[COUNT_NUM_PARAM_3].value);
  
  bool reset = false;
  bool reset_2 = false;
  bool reset_3 = false;  
  
    if (reset_trigger.process(params[RST_BUTTON1].value)  || (reset_trigger.process(inputs[RESET_IN].value))){
        reset = true;
        count = 0;
        outputs[OUTPUT].value = 0; 
        resetLight1 = 1.0;

    }  

  resetLight1 -= resetLight1 / lightLambda / engineGetSampleRate();
  lights[RESET_LIGHT1].value = resetLight1;

  if (reset == false){
		if (clock_trigger.process(inputs[CLK_IN].value) && count <= count_limit)
					count++;	
  }
  if (count == count_limit) outputs[OUTPUT].value = 10.0;
  if (count > count_limit){
    count = 0;
    outputs[OUTPUT].value = 0; 
  }
  ///////////// counter 2
  if (reset_trigger_2.process(params[RST_BUTTON2].value)  || (reset_trigger_2.process(inputs[RESET_IN_2].value))){
    reset_2 = true;
    count_2 = 0;
    outputs[OUTPUT_2].value = 0;
    resetLight2 = 1.0;
  } 
  resetLight2 -= resetLight2 / lightLambda / engineGetSampleRate();
  lights[RESET_LIGHT2].value = resetLight2;

  if (reset_2 == false){
		if (clock_trigger_2.process(inputs[CLK_IN_2].value) && count_2 <= count_limit_2)
					count_2++;	
  }
  if (count_2 == count_limit_2) outputs[OUTPUT_2].value = 10.0;
  if (count_2 > count_limit_2){
    count_2 = 0;
    outputs[OUTPUT_2].value = 0; 
  }  

  ///////////// counter 3
  if (reset_trigger_3.process(params[RST_BUTTON3].value)  || (reset_trigger_3.process(inputs[RESET_IN_3].value))){
    reset_3 = true;
    count_3 = 0;
    outputs[OUTPUT_3].value = 0;
    resetLight3 = 1.0;
  } 
  resetLight3 -= resetLight3 / lightLambda / engineGetSampleRate();
  lights[RESET_LIGHT3].value = resetLight3;

  if (reset_3 == false){
		if (clock_trigger_3.process(inputs[CLK_IN_3].value) && count_3 <= count_limit_3)
					count_3++;	
  }
  if (count_3 == count_limit_3) outputs[OUTPUT_3].value = 10.0;
  if (count_3 > count_limit_3){
    count_3 = 0;
    outputs[OUTPUT_3].value = 0; 
  }  

}

///////////////////////////////////
struct NumberDisplayWidget : TransparentWidget {

  int *value;
  std::shared_ptr<Font> font;

  NumberDisplayWidget() {
    font = Font::load(assetPlugin(plugin, "res/Segment7Standard.ttf"));
  };

  void draw(NVGcontext *vg) override
  {
    // Background
    NVGcolor backgroundColor = nvgRGB(0x20, 0x20, 0x20);
    NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(vg, backgroundColor);
    nvgFill(vg);
    nvgStrokeWidth(vg, 1.0);
    nvgStrokeColor(vg, borderColor);
    nvgStroke(vg);    
    // text 
    nvgFontSize(vg, 18);
    nvgFontFaceId(vg, font->handle);
    nvgTextLetterSpacing(vg, 2.5);

    std::stringstream to_display;   
    to_display << std::right  << std::setw(2) << *value;

    Vec textPos = Vec(4.0f, 17.0f); 

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "\\\\", NULL);

    textColor = nvgRGB(0xf0, 0x00, 0x00);
    nvgFillColor(vg, textColor);
    nvgText(vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
  }
};
////////////////////////////////////


StepsWidget::StepsWidget() {
	Steps *module = new Steps();
	setModule(module);
	//box.size = Vec(15*4, 380);
  	box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
  
	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin,"res/Steps.svg")));
		addChild(panel);
	}
  
 //SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
  // counter 1
  //COUNT DISPLAY
    NumberDisplayWidget *display1 = new NumberDisplayWidget();
    display1->box.pos = Vec(10,50);
    display1->box.size = Vec(30, 20);
    display1->value = &module->count;
    addChild(display1);
  //STEPS DISPLAY  
    NumberDisplayWidget *display2 = new NumberDisplayWidget();
    display2->box.pos = Vec(50,50);
    display2->box.size = Vec(30, 20);
    display2->value = &module->count_limit;
    addChild(display2);

   int group_offset = 100;

    addParam(createParam<LEDBezel>(Vec(5, 82), module, Steps::RST_BUTTON1 , 0.0, 1.0, 0.0));
    addChild(createLight<LedLight<RedLight>>(Vec(5+2.2, 82+2.3), module, Steps::RESET_LIGHT1));

    addParam(createParam<as_KnobBlack>(Vec(43, 73), module, Steps::COUNT_NUM_PARAM, 1.0, 64.0, 1.0)); 

    addInput(createInput<as_PJ301MPort>(Vec(3, 120), module, Steps::RESET_IN));
    addInput(createInput<as_PJ301MPort>(Vec(33, 120), module, Steps::CLK_IN));
    addOutput(createOutput<as_PJ301MPort>(Vec(63, 120), module, Steps::OUTPUT));
  
  // counter 2
  //COUNT DISPLAY
    NumberDisplayWidget *display3 = new NumberDisplayWidget();
    display3->box.pos = Vec(10,50 + group_offset);
    display3->box.size = Vec(30, 20);
    display3->value = &module->count_2;
    addChild(display3);
  //STEPS DISPLAY  
    NumberDisplayWidget *display4 = new NumberDisplayWidget();
    display4->box.pos = Vec(50,50 + group_offset);
    display4->box.size = Vec(30, 20);
    display4->value = &module->count_limit_2;
    addChild(display4);

    addParam(createParam<LEDBezel>(Vec(5, 82+ group_offset), module, Steps::RST_BUTTON2 , 0.0, 1.0, 0.0));
    addChild(createLight<LedLight<RedLight>>(Vec(5+2.2, 82+2.3+ group_offset), module, Steps::RESET_LIGHT2));

    addParam(createParam<as_KnobBlack>(Vec(43, 73 + group_offset), module, Steps::COUNT_NUM_PARAM_2, 1.0, 64.0, 1.0)); 

    addInput(createInput<as_PJ301MPort>(Vec(3, 120 + group_offset), module, Steps::RESET_IN_2));
    addInput(createInput<as_PJ301MPort>(Vec(33, 120 + group_offset), module, Steps::CLK_IN_2));
    addOutput(createOutput<as_PJ301MPort>(Vec(63, 120 + group_offset), module, Steps::OUTPUT_2));

  // counter 3
  //COUNT DISPLAY
    NumberDisplayWidget *display5 = new NumberDisplayWidget();
    display5->box.pos = Vec(10,50 + group_offset*2);
    display5->box.size = Vec(30, 20);
    display5->value = &module->count_3;
    addChild(display5);
  //STEPS DISPLAY  
    NumberDisplayWidget *display6 = new NumberDisplayWidget();
    display6->box.pos = Vec(50,50 + group_offset*2);
    display6->box.size = Vec(30, 20);
    display6->value = &module->count_limit_3;
    addChild(display6);

    addParam(createParam<LEDBezel>(Vec(5, 82+ group_offset*2), module, Steps::RST_BUTTON3 , 0.0, 1.0, 0.0));
    addChild(createLight<LedLight<RedLight>>(Vec(5+2.2, 82+2.3+ group_offset*2), module, Steps::RESET_LIGHT3));

    addParam(createParam<as_KnobBlack>(Vec(43, 73 + group_offset*2), module, Steps::COUNT_NUM_PARAM_3, 1.0, 64.0, 1.0)); 

    addInput(createInput<as_PJ301MPort>(Vec(3, 120 + group_offset*2), module, Steps::RESET_IN_3));
    addInput(createInput<as_PJ301MPort>(Vec(33, 120 + group_offset*2), module, Steps::CLK_IN_3));
    addOutput(createOutput<as_PJ301MPort>(Vec(63, 120 + group_offset*2), module, Steps::OUTPUT_3));	  
}
