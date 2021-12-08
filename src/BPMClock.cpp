//**************************************************************************************
//
//BPM Clock module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Based on code taken from Master Clock Module VCV Module Strum 2017 https://github.com/Strum/Strums_Mental_VCV_Modules
//**************************************************************************************

#include "AS.hpp"
//#include "dsp/digital.hpp"

#include <sstream>
#include <iomanip>

struct LFOGenerator {
	float phase = 0.0f;
	float pw = 0.5f;
	float freq = 1.0f;
	dsp::SchmittTrigger resetTrigger;
	void setFreq(float freq_to_set)
  {
    freq = freq_to_set;
  }		
	void step(float dt) {
		float deltaPhase = fminf(freq * dt, 0.5f);
		phase += deltaPhase;
		if (phase >= 1.0f)
			phase -= 1.0f;
	}
  void setReset(float reset) {
		if (resetTrigger.process(reset)) {
			phase = 0.0f;
		}
	}
	float sqr() {
		float sqr = phase < pw ? 1.0f : -1.0f;
		return sqr;
	}
};

struct BPMClock : Module {
	enum ParamIds {    
    TEMPO_PARAM,
    MODE_PARAM,    
    TIMESIGTOP_PARAM,
    TIMESIGBOTTOM_PARAM,
    RESET_SWITCH,
    RUN_SWITCH,    
		NUM_PARAMS
	};  
	enum InputIds { 
    RUN_CV,
    RESET_INPUT,    
		NUM_INPUTS
	};
	enum OutputIds {
		BEAT_OUT,
    EIGHTHS_OUT,
    SIXTEENTHS_OUT,
    BAR_OUT,
    RESET_OUTPUT, 
    RUN_OUTPUT,       
		NUM_OUTPUTS
	};
  enum LightIds {
		RESET_LED,
    RUN_LED,
		NUM_LIGHTS
	}; 
  
  LFOGenerator clock;
  
  dsp::SchmittTrigger eighths_trig;
	dsp::SchmittTrigger quarters_trig;
  dsp::SchmittTrigger bars_trig;
  dsp::SchmittTrigger run_button_trig;
  dsp::SchmittTrigger ext_run_trig;
	dsp::SchmittTrigger reset_btn_trig;
  dsp::SchmittTrigger reset_ext_trig;
  dsp::SchmittTrigger bpm_mode_trig;

  dsp::PulseGenerator resetPulse;
  bool reset_pulse = false;

  dsp::PulseGenerator runPulse;
  bool run_pulse = false;

  // PULSES FOR TRIGGER OUTPUTS INSTEAD OF GATES
	dsp::PulseGenerator clockPulse8s;
  bool pulse8s = false;
  dsp::PulseGenerator clockPulse4s;
  bool pulse4s = false;
	dsp::PulseGenerator clockPulse1s;
  bool pulse1s = false;
	dsp::PulseGenerator clockPulse16s;
  bool pulse16s = false;

	float trigger_length = 0.0001f;

  const float lightLambda = 0.075f;
  float resetLight = 0.0f;

  bool running = true;
  
  int eighths_count = 0;
	int quarters_count = 0;
  int bars_count = 0;
  
  float tempo =120.0f;
  int time_sig_top, time_sig_bottom = 0;
  int time_sig_bottom_old = 0;
  float frequency = 2.0f;
  int quarters_count_limit = 4;
  int eighths_count_limit = 2;
  int bars_count_limit = 16;
  
  //float min_bpm = 40.0f;
  //float max_bpm = 250.0f;

  float min_bpm = 30.0f;
  float max_bpm = 300.0f;

	BPMClock() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(BPMClock::TEMPO_PARAM, min_bpm, max_bpm, 120.0f, "Tempo", " BPM");
    configParam(BPMClock::MODE_PARAM, 0.0f, 1.0f, 1.0f, "Mode: Regular/Extended");
    configParam(BPMClock::TIMESIGTOP_PARAM,2.0f, 15.0f, 4.0f, "Time Signature Top");
    configParam(BPMClock::TIMESIGBOTTOM_PARAM,0.0f, 3.0f, 1.0f, "Time Signature Bottom");
    configParam(BPMClock::RUN_SWITCH , 0.0f, 1.0f, 0.0f, "Run");
    configParam(BPMClock::RESET_SWITCH , 0.0f, 1.0f, 0.0f, "Reset");

		//New in V2, config switches and ports info without displaying values
		//inputs
		configInput(RUN_CV, "Run CV");
		configInput(RESET_INPUT, "Reset CV");
		//Outputs
		configOutput(BEAT_OUT, "Beat");
		configOutput(EIGHTHS_OUT, "Eights");
		configOutput(SIXTEENTHS_OUT, "Sixteenths");
		configOutput(BAR_OUT, "Bar");
		configOutput(RESET_OUTPUT, "Reset");
		configOutput(RUN_OUTPUT, "Run");

  }

  void process(const ProcessArgs &args) override {

    if (run_button_trig.process(params[RUN_SWITCH].getValue()) || ext_run_trig.process(inputs[RUN_CV].getVoltage())){
        running = !running;
            runPulse.trigger(0.01f);
    }

    lights[RUN_LED].value = running ? 1.0f : 0.0f;

    run_pulse = runPulse.process(1.0 / args.sampleRate);
    outputs[RUN_OUTPUT].setVoltage((run_pulse ? 10.0f : 0.0f));
    /*
    if (params[MODE_PARAM].getValue()){
      min_bpm = 40.0f;
      max_bpm = 250.0f;
      //regular 40 to 250 bpm mode
      tempo = std::round(params[TEMPO_PARAM].getValue());
    }else{
      min_bpm = 30.0f;
      max_bpm = 300.0f;
      //extended 30 to 300 mode
      tempo = std::round(rescale(params[TEMPO_PARAM].getValue(),40.0f,250.0f, 30.0f, 300.0f) );
    }
    */
    tempo = std::round(params[TEMPO_PARAM].getValue());

    time_sig_top = std::round(params[TIMESIGTOP_PARAM].getValue());
    time_sig_bottom = std::round(params[TIMESIGBOTTOM_PARAM].getValue());
    time_sig_bottom = std::pow(2,time_sig_bottom+1);
    
  
    frequency = tempo/60.0f;

    //RESET TRIGGER
    if(reset_ext_trig.process(inputs[RESET_INPUT].getVoltage()) || reset_btn_trig.process(params[RESET_SWITCH].getValue())) {
      clock.setReset(1.0f);
      eighths_count = 0;
      quarters_count = 0;
      bars_count = 0;
      resetLight = 1.0;
      resetPulse.trigger(0.01f);
    }

    resetLight -= resetLight / lightLambda / args.sampleRate;
    lights[RESET_LED].value = resetLight;
    reset_pulse = resetPulse.process(1.0 / args.sampleRate);
    outputs[RESET_OUTPUT].setVoltage((reset_pulse ? 10.0f : 0.0f));


    if(!running){

      eighths_count = 0;
      quarters_count = 0;
      bars_count = 0; 
      outputs[BAR_OUT].setVoltage(0.0f);
      outputs[BEAT_OUT].setVoltage(0.0f);
      outputs[EIGHTHS_OUT].setVoltage(0.0f);
      outputs[SIXTEENTHS_OUT].setVoltage(0.0f);

    }else{

      if (time_sig_top == time_sig_bottom){
        quarters_count_limit = 4;
        eighths_count_limit = 2;
        bars_count_limit = 16; 
        clock.setFreq(frequency*4);   
      }else{
        //clock divisions
        if(time_sig_bottom == 4){
          //debug("time sig bottom = %i", time_sig_bottom);
          quarters_count_limit = 4;
          eighths_count_limit = 2;
          bars_count_limit = time_sig_top * 4; 
          clock.setFreq(frequency*4);
        }
        if(time_sig_bottom == 8){
          quarters_count_limit = 4;
          eighths_count_limit = 2;
          bars_count_limit = time_sig_top * 2;
          clock.setFreq(frequency*4);
        }
        if((time_sig_top % 3) == 0){
          quarters_count_limit = 6;
          eighths_count_limit = 2;
          bars_count_limit = (time_sig_top/3) * 6;
          clock.setFreq(frequency*6);
        }      
      }
    }
  
    if(running){
      clock.step(1.0 / args.sampleRate);

      //16ths
      float clock16s = clamp(10.0f * clock.sqr(), 0.0f, 10.0f);

      if(clock16s>0){
        clockPulse16s.trigger(trigger_length);
      }

      //8ths
      if (eighths_trig.process(clock.sqr()) && eighths_count <= eighths_count_limit){
        eighths_count++;
      }
      if (eighths_count >= eighths_count_limit){
        eighths_count = 0;    
      }

      if(eighths_count == 0){
        clockPulse8s.trigger(trigger_length);
      }
      //4ths
      if (quarters_trig.process(clock.sqr()) && quarters_count <= quarters_count_limit){
        quarters_count++;
      }
      if (quarters_count >= quarters_count_limit){
        quarters_count = 0;    
      }

      if(quarters_count == 0){
        clockPulse4s.trigger(trigger_length);
      }
      
      //bars
      if (bars_trig.process(clock.sqr()) && bars_count <= bars_count_limit){
        bars_count++;
      }
      if (bars_count >= bars_count_limit){
        bars_count = 0;    
      }

      if(bars_count == 0){
        clockPulse1s.trigger(trigger_length);
      }
    }

      pulse1s = clockPulse1s.process(1.0 / args.sampleRate);
      pulse4s = clockPulse4s.process(1.0 / args.sampleRate);
      pulse8s = clockPulse8s.process(1.0 / args.sampleRate);
      pulse16s = clockPulse16s.process(1.0 / args.sampleRate);
      
      outputs[BAR_OUT].setVoltage((pulse1s ? 10.0f : 0.0f));
      outputs[BEAT_OUT].setVoltage((pulse4s ? 10.0f : 0.0f));
      outputs[EIGHTHS_OUT].setVoltage((pulse8s ? 10.0f : 0.0f));
      outputs[SIXTEENTHS_OUT].setVoltage((pulse16s ? 10.0f : 0.0f));

  }

  json_t *dataToJson() override{
		json_t *rootJ = json_object();
    json_t *button_statesJ = json_array();
		json_t *button_stateJ = json_integer((int)running);
		json_array_append_new(button_statesJ, button_stateJ);		
		json_object_set_new(rootJ, "run", button_statesJ);    
    return rootJ;
  }
  
  void dataFromJson(json_t *rootJ) override{
    json_t *button_statesJ = json_object_get(rootJ, "run");
		if (button_statesJ){			
				json_t *button_stateJ = json_array_get(button_statesJ,0);
				if (button_stateJ)
					running = !!json_integer_value(button_stateJ);			
		}  
  } 

};

////////////////////////////////////
struct BpmDisplayWidget : TransparentWidget {
  float *value = NULL;
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
      to_display << std::setw(3) << *value;

      Vec textPos = Vec(4.0f, 17.0f); 

      NVGcolor textColor = nvgRGB(0xf0, 0x00, 0x00);
      nvgFillColor(args.vg, textColor);
      nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
    }
  }
};
////////////////////////////////////
struct SigDisplayWidget : TransparentWidget {

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
      to_display << std::setw(2) << *value;

      Vec textPos = Vec(3.0f, 17.0f); 

      NVGcolor textColor = nvgRGB(0xf0, 0x00, 0x00);
      nvgFillColor(args.vg, textColor);
      nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
    }
  }
};
//////////////////////////////////

struct BPMClockWidget : ModuleWidget { 

  BPMClockWidget(BPMClock *module) {
    
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BPMClock.svg")));

    //SCREWS
    addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    //BPM DISPLAY 
    BpmDisplayWidget *display = new BpmDisplayWidget();
    display->box.pos = Vec(23,45);
    display->box.size = Vec(45, 20);
    if (module) {
      display->value = &module->tempo;
    }
    addChild(display); 
    //TEMPO KNOB
    addParam(createParam<as_KnobBlackSnap>(Vec(8, 69), module, BPMClock::TEMPO_PARAM));
    //OLD/NEW SWITCH FROM 40-250 TO 30-300
   // addParam(createParam<as_CKSS>(Vec(67, 77), module, BPMClock::MODE_PARAM));
    //SIG TOP DISPLAY 
    SigDisplayWidget *display2 = new SigDisplayWidget();
    display2->box.pos = Vec(54,123);
    display2->box.size = Vec(30, 20);
    if (module) {
      display2->value = &module->time_sig_top;
    }
    addChild(display2);
    //SIG TOP KNOB
    addParam(createParam<as_Knob>(Vec(8, 110), module, BPMClock::TIMESIGTOP_PARAM));
    //SIG BOTTOM DISPLAY    
    SigDisplayWidget *display3 = new SigDisplayWidget();
    display3->box.pos = Vec(54,155);
    display3->box.size = Vec(30, 20);
    if (module) {
      display3->value = &module->time_sig_bottom;
    }
    addChild(display3); 
    //SIG BOTTOM KNOB
    addParam(createParam<as_Knob>(Vec(8, 150), module, BPMClock::TIMESIGBOTTOM_PARAM));
    //RESET & RUN LEDS
    addParam(createParam<LEDBezel>(Vec(33.5, 202), module, BPMClock::RUN_SWITCH ));
    addChild(createLight<LEDBezelLight<RedLight>>(Vec(35.7, 204.3), module, BPMClock::RUN_LED));

    addParam(createParam<LEDBezel>(Vec(33.5, 241), module, BPMClock::RESET_SWITCH ));
    addChild(createLight<LEDBezelLight<RedLight>>(Vec(35.7, 243.2), module, BPMClock::RESET_LED));
    //RESET INPUT
    addInput(createInput<as_PJ301MPort>(Vec(6, 240), module, BPMClock::RESET_INPUT));
    //RESET OUTPUT
    addOutput(createOutput<as_PJ301MPortGold>(Vec(59, 240), module, BPMClock::RESET_OUTPUT));
    //TEMPO OUTPUTS
    addOutput(createOutput<as_PJ301MPortGold>(Vec(6, 280), module, BPMClock::BAR_OUT));
    addOutput(createOutput<as_PJ301MPortGold>(Vec(59, 280), module, BPMClock::BEAT_OUT));
    addOutput(createOutput<as_PJ301MPortGold>(Vec(6, 320), module, BPMClock::EIGHTHS_OUT));
    addOutput(createOutput<as_PJ301MPortGold>(Vec(59, 320), module, BPMClock::SIXTEENTHS_OUT));

    //RUN CV
    addInput(createInput<as_PJ301MPort>(Vec(6, 200), module, BPMClock::RUN_CV));
    //RUN TRIGGER OUTPUT
    addOutput(createOutput<as_PJ301MPortGold>(Vec(59, 200), module, BPMClock::RUN_OUTPUT));

  }
};


Model *modelBPMClock = createModel<BPMClock, BPMClockWidget>("BPMClock");