//**************************************************************************************
//
//BPM Calculator module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//### BPM detect portions of code by Tomasz Sosnowski - KoralFX
//**************************************************************************************

#include "AS.hpp"

//#include "dsp/digital.hpp"

#include <sstream>
#include <iomanip>


struct BPMCalc : Module {
	enum ParamIds {    
    TEMPO_PARAM,      
		NUM_PARAMS
	};  
	enum InputIds {
    CLOCK_INPUT,  
		NUM_INPUTS
	};
	enum OutputIds {  
    ENUMS(MS_OUTPUT, 16),   
		NUM_OUTPUTS
	};
  enum LightIds {
    CLOCK_LOCK_LIGHT,
    CLOCK_LIGHT,
		NUM_LIGHTS
	};

  //bpm detector variables
  bool inMemory = false;
  bool beatLock = false;
  float beatTime = 0.0f;
  int beatCount = 0;
  int beatCountMemory = 0;
  float beatOld = 0.0f;

  std::string tempo = "---";

  dsp::SchmittTrigger clockTrigger;
  dsp::PulseGenerator LightPulse;
  bool pulse = false;

  //calculator variables
  float bpm = 120;
  float last_bpm = 0;
  float millisecs = 60000;
  float mult = 1000;
  float millisecondsPerBeat;
  float millisecondsPerMeasure;
  float bar = 1.0f;

  float secondsPerBeat = 0.0f;
	float secondsPerMeasure = 0.0f;
  //ms variables
  float half_note_d = 1.0f;
  float half_note = 1.0f;
  float half_note_t =1.0f;

  float qt_note_d = 1.0f;
  float qt_note = 1.0f;
  float qt_note_t = 1.0f;

  float eight_note_d = 1.0f;
  float eight_note =1.0f;
  float eight_note_t = 1.0f;

  float sixth_note_d =1.0f;
  float sixth_note = 1.0f;
  float sixth_note_t = 1.0f;

  float trth_note_d = 1.0f;
  float trth_note = 1.0f;
  float trth_note_t = 1.0f;
  //hz variables
  float hz_bar = 1.0f;
  float half_hz_d = 1.0f;
  float half_hz = 1.0f;
  float half_hz_t = 1.0f;

  float qt_hz_d = 1.0f;
  float qt_hz = 1.0f;
  float qt_hz_t = 1.0f;

  float eight_hz_d = 1.0f;
  float eight_hz = 1.0f;
  float eight_hz_t = 1.0f;

  float sixth_hz_d = 1.0f;
  float sixth_hz = 1.0f;
  float sixth_hz_t = 1.0f;

  float trth_hz_d = 1.0f;
  float trth_hz = 1.0f;
  float trth_hz_t = 1.0f;


  void calculateValues(float bpm){ 

        millisecondsPerBeat = millisecs/bpm;
        millisecondsPerMeasure = millisecondsPerBeat * 4;

        secondsPerBeat = 60 / bpm;
        secondsPerMeasure = secondsPerBeat * 4;

        bar = (millisecondsPerMeasure);

        half_note_d = ( millisecondsPerBeat * 3 );
        half_note = ( millisecondsPerBeat * 2 );
        half_note_t = ( half_note * 2 / 3 );

        qt_note_d = ( millisecondsPerBeat / 2 ) * 3;
        qt_note = millisecondsPerBeat;
        qt_note_t =  half_note  / 3;

        eight_note_d = ( millisecondsPerBeat / 4 ) * 3;
        eight_note = millisecondsPerBeat / 2;
        eight_note_t = millisecondsPerBeat / 3;

        sixth_note = millisecondsPerBeat / 4;
        sixth_note_d = ( sixth_note ) * 1.5;
        sixth_note_t = millisecondsPerBeat / 6;
      
        trth_note = millisecondsPerBeat / 8;
        trth_note_d = ( trth_note ) * 1.5;
        trth_note_t = trth_note * 2 / 3;
        //hz measures
        hz_bar = (1/secondsPerMeasure);

        half_hz_d = mult / half_note_d;
        half_hz = mult / half_note;
        half_hz_t = mult / half_note_t;

        qt_hz_d = mult / qt_note_d;
        qt_hz = mult / qt_note;
        qt_hz_t = mult / qt_note_t;

        eight_hz_d = mult / eight_note_d;
        eight_hz = mult / eight_note;
        eight_hz_t = mult / eight_note_t;

        sixth_hz_d = mult / sixth_note_d;
        sixth_hz = mult / sixth_note;
        sixth_hz_t = mult / sixth_note_t;

        trth_hz_d = mult / trth_note_d;
        trth_hz = mult / trth_note;
        trth_hz_t = mult / trth_note_t;

         last_bpm = bpm;

  }

  void refreshDetector() {

      inMemory = false;
      beatLock = false;
      beatTime = 0.0f;
      beatCount = 0;
      beatCountMemory = 0;
      beatOld = 0.0f;
      tempo = "---";
      pulse = false;

	}
  
	BPMCalc() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
      configParam(BPMCalc::TEMPO_PARAM, 30.0f, 300.0f, 120.0f, "Tempo", " BPM", 0.0f);
      configInput(CLOCK_INPUT, "Clock");
      //Outputs
      configOutput(MS_OUTPUT +0, "NOTE: 1");
      configOutput(MS_OUTPUT +1, "NOTE: ·1/2");
      configOutput(MS_OUTPUT +2, "NOTE: 1/2");
      configOutput(MS_OUTPUT +3, "NOTE: t1/2");
      configOutput(MS_OUTPUT +4, "NOTE: ·1/4");
      configOutput(MS_OUTPUT +5, "NOTE: 1/4");
      configOutput(MS_OUTPUT +6, "NOTE: t1/4");
      configOutput(MS_OUTPUT +7, "NOTE: ·1/8");
      configOutput(MS_OUTPUT +8, "NOTE: 1/8");
      configOutput(MS_OUTPUT +9, "NOTE: t1/8");
      configOutput(MS_OUTPUT +10, "NOTE: ·1/16");
      configOutput(MS_OUTPUT +11, "NOTE: 1/16");
      configOutput(MS_OUTPUT +12, "NOTE: t1/16");
      configOutput(MS_OUTPUT +13, "NOTE: ·1/32");
      configOutput(MS_OUTPUT +14, "NOTE: 1/32");
      configOutput(MS_OUTPUT +15, "NOTE: t1/32");

    }

  void onReset() override {

    refreshDetector();

	}

	void onInitialize() {

    refreshDetector();

	}

  void process(const ProcessArgs &args) override {

    //BPM detection code
    float deltaTime = args.sampleTime;

    if ( inputs[CLOCK_INPUT].isConnected() ) {

      float clockInput = inputs[CLOCK_INPUT].getVoltage();
      //A rising slope
      if ( ( clockTrigger.process( inputs[CLOCK_INPUT].getVoltage() ) ) && !inMemory ) {
        beatCount++;
        if(!beatLock){
          lights[CLOCK_LIGHT].value = 1.0f;
          LightPulse.trigger( 0.1f );
        }
        inMemory = true;
      
        //BPM is locked
        if ( beatCount == 2 ) {
          lights[CLOCK_LOCK_LIGHT].value = 1.0f;
          beatLock = true;
          beatOld = beatTime;
        }
        //BPM is lost
        if ( beatCount > 2 ) {

          if ( fabs( beatOld - beatTime ) > 0.0005f ) {
            beatLock = false;
            beatCount = 0;
            lights[CLOCK_LOCK_LIGHT].value = 0.0f;
            tempo = "---";
          }

        }

        beatTime = 0;

      }

      //Falling slope
      if ( clockInput <= 0 && inMemory ) {
        inMemory = false;
      }
      //When BPM is locked
      if ( beatLock ) {
        bpm = (int)round( 60 / beatOld );
        tempo = std::to_string( (int)round(bpm) );
        if(bpm!=last_bpm){
          if(bpm<999){
            calculateValues(bpm);
          }else{
            tempo = "OOR";
          }
        }
    
      } //end of beatLock routine

      beatTime += deltaTime;

      //when beat is lost
      if ( beatTime > 2 ) {
        beatLock = false;
        beatCount = 0;
        lights[CLOCK_LOCK_LIGHT].value = 0.0f;
        tempo = "---";
      }
      beatCountMemory = beatCount;

    } else {
      beatLock = false;
      beatCount = 0;
      //tempo = "OFF";
      lights[CLOCK_LOCK_LIGHT].value = 0.0f;
      //caluculate with knob value instead of bmp detector value
      bpm = params[TEMPO_PARAM].getValue();
      if (bpm<30){
        bpm = 30;
      }
      bpm = (int)round(bpm);
      tempo = std::to_string( (int)round(bpm) );
      if(bpm!=last_bpm){
          calculateValues(bpm);
      }

    }

    pulse = LightPulse.process( 1.0 / args.sampleRate );
    lights[CLOCK_LIGHT].value = (pulse ? 1.0f : 0.0f);

    //OUTPUTS: MS to 10V scaled values
    outputs[MS_OUTPUT+0].setVoltage(rescale(bar,0.0f,10000.0f,0.0f,10.0f));
    outputs[MS_OUTPUT+1].setVoltage(rescale(half_note_d,0.0f,10000.0f,0.0f,10.0f));

    outputs[MS_OUTPUT+2].setVoltage(rescale(half_note,0.0f,10000.0f,0.0f,10.0f));
    outputs[MS_OUTPUT+3].setVoltage(rescale(half_note_t,0.0f,10000.0f,0.0f,10.0f));

    outputs[MS_OUTPUT+4].setVoltage(rescale(qt_note_d,0.0f,10000.0f,0.0f,10.0f));
    outputs[MS_OUTPUT+5].setVoltage(rescale(qt_note,0.0f,10000.0f,0.0f,10.0f));

    outputs[MS_OUTPUT+6].setVoltage(rescale(qt_note_t,0.0f,10000.0f,0.0f,10.0f));
    outputs[MS_OUTPUT+7].setVoltage(rescale(eight_note_d,0.0f,10000.0f,0.0f,10.0f));

    outputs[MS_OUTPUT+8].setVoltage(rescale(eight_note,0.0f,10000.0f,0.0f,10.0f));
    outputs[MS_OUTPUT+9].setVoltage(rescale(eight_note_t,0.0f,10000.0f,0.0f,10.0f));

    outputs[MS_OUTPUT+10].setVoltage(rescale(sixth_note_d,0.0f,10000.0f,0.0f,10.0f));
    outputs[MS_OUTPUT+11].setVoltage(rescale(sixth_note,0.0f,10000.0f,0.0f,10.0f));

    outputs[MS_OUTPUT+12].setVoltage(rescale(sixth_note_t,0.0f,10000.0f,0.0f,10.0f));
    outputs[MS_OUTPUT+13].setVoltage(rescale(trth_note_d,0.0f,10000.0f,0.0f,10.0f));

    outputs[MS_OUTPUT+14].setVoltage(rescale(trth_note,0.0f,10000.0f,0.0f,10.0f));
    
    outputs[MS_OUTPUT+15].setVoltage(rescale(trth_note_t,0.0f,10000.0f,0.0f,10.0f));

  }

};

////////////////////////////////////
struct TempodisplayWidget : TransparentWidget {

  std::string *value = NULL;
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

      Vec textPos = Vec(14.0f, 17.0f); 

      NVGcolor textColor = nvgRGB(0xf0, 0x00, 0x00);
      nvgFillColor(args.vg, textColor);
      nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
    }
  }
};


///////////////////////////////////

struct TxtDisplay : TransparentWidget{
  BPMCalc *module;
  int frame = 0;
  const int v_s = 14;
  const int h_off = 60;
  const int hz_txt_off = 125;
  std::shared_ptr<Font> font;
  std::string fontPath = asset::plugin(pluginInstance, "res/saxmono.ttf");


	void drawLayer(const DrawArgs& args, int layer) override {
		if (layer != 1){
			return;
		}
    if (++frame >= 16){
      frame = 0;
    }
    if (module) {
      font = APP->window->loadFont(fontPath);
      if(font){
        nvgFontSize(args.vg, 13);
        nvgFontFaceId(args.vg, font->handle);
        nvgTextLetterSpacing(args.vg, 0);
        nvgTextAlign(args.vg, NVG_ALIGN_LEFT);
        NVGcolor  textColor = nvgRGB(0xf0, 0x00, 0x00);
        nvgFillColor(args.vg, textColor);
        //note texts
        nvgText(args.vg, 0, 0+v_s * 0,  "     1:", NULL);
        nvgText(args.vg, 0, 0+v_s * 1,  "  •1/2:", NULL);
        nvgText(args.vg, 0, 0+v_s * 2,  "   1/2:", NULL);
        nvgText(args.vg, 0, 0+v_s * 3,  "  t1/2:", NULL);
        nvgText(args.vg, 0, 0+v_s * 4,  "  •1/4:", NULL);
        nvgText(args.vg, 0, 0+v_s * 5,  "   1/4:", NULL);
        nvgText(args.vg, 0, 0+v_s * 6,  "  t1/4:", NULL);
        nvgText(args.vg, 0, 0+v_s * 7,  "  •1/8:", NULL);
        nvgText(args.vg, 0, 0+v_s * 8,  "   1/8:", NULL);
        nvgText(args.vg, 0, 0+v_s * 9,  "  t1/8:", NULL);
        nvgText(args.vg, 0, 0+v_s * 10, " •1/16:", NULL);
        nvgText(args.vg, 0, 0+v_s * 11, "  1/16:", NULL);
        nvgText(args.vg, 0, 0+v_s * 12, " t1/16:", NULL);
        nvgText(args.vg, 0, 0+v_s * 13, " •1/32:", NULL);
        nvgText(args.vg, 0, 0+v_s * 14, "  1/32:", NULL);
        nvgText(args.vg, 0, 0+v_s * 15, " t1/32:", NULL);

        std::stringstream to_display;
        to_display << std::fixed;
        to_display.precision(0);
        to_display.fill (' ');
        //bar
        to_display.width(4);
        to_display <<  module->bar  << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 0,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        //1/2
        to_display.width(4);
        to_display << module->half_note_d << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 1,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->half_note << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 2,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->half_note_t << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 3,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        //1/4
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->qt_note_d << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 4,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->qt_note << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 5,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->qt_note_t << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 6,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        //1/8
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->eight_note_d << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 7,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->eight_note << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 8,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->eight_note_t << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 9,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        //1/16
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->sixth_note_d << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 10,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->sixth_note << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 11,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->sixth_note_t << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 12,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        //1/32
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->trth_note_d << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 13,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->trth_note << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 14,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(4);
        to_display << module->trth_note_t << " ms.";
        nvgText(args.vg, 0 + h_off, 0 + v_s * 15,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");

        to_display.precision(2);
        //hz bar
        to_display.width(5);
        to_display <<  module->hz_bar  << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 0,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        //hz 1/2
        to_display.width(5);
        to_display << module->half_hz_d << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 1,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->half_hz << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 2,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->half_hz_t << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 3,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        //hz 1/4
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->qt_hz_d << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 4,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->qt_hz << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 5,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->qt_hz_t << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 6,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        //hz 1/8
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->eight_hz_d << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 7,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->eight_hz << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 8,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->eight_hz_t << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 9,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        //hz 1/16
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->sixth_hz_d << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 10,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->sixth_hz << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 11,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->sixth_hz_t << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 12,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        //hz 1/32
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->trth_hz_d << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 13,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->trth_hz << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 14,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
        to_display.width(5);
        to_display << module->trth_hz_t << " hz.";
        nvgText(args.vg, 0 + hz_txt_off, 0 + v_s * 15,  to_display.str().c_str(), NULL);
        to_display.clear(); to_display.str("");
      }
    }
  }
};

//////////////////////////////////

struct BPMCalcWidget : ModuleWidget { 

  BPMCalcWidget(BPMCalc *module) {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BPMCalc.svg")));

    //SCREWS
    addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    //BPM DETECTOR PORT
    addInput(createInput<as_PJ301MPort>(Vec(20, 52), module, BPMCalc::CLOCK_INPUT));

    //BPM DISPLAY 
    TempodisplayWidget *display = new TempodisplayWidget();
    display->box.pos = Vec(73,54);
    display->box.size = Vec(55, 20);
    if (module) {
      display->value = &module->tempo;
    }
    addChild(display);
    //DETECTOR LEDS
    addChild(createLight<DisplayLedLight<RedLight>>(Vec(77, 56), module, BPMCalc::CLOCK_LOCK_LIGHT));
    addChild(createLight<DisplayLedLight<RedLight>>(Vec(77, 66), module, BPMCalc::CLOCK_LIGHT)); 
    //TEMPO KNOB
    addParam(createParam<as_KnobBlackSnap>(Vec(156, 45), module, BPMCalc::TEMPO_PARAM));
    //CALCULATOR DISPLAY 
    TxtDisplay *display1 = new TxtDisplay();
    display1->module = module;
    display1->box.pos = Vec(7, 120);
    display1->box.size = Vec(190, 240);
    addChild(display1);
    //MS outputs
    int const out_offset = 40;
    // 1 - ·1/2
    addOutput(createOutput<as_PJ301MPortGold>(Vec(220, 50), module, BPMCalc::MS_OUTPUT + 0));
    addOutput(createOutput<as_PJ301MPortGold>(Vec(260, 50), module, BPMCalc::MS_OUTPUT + 1));
    // 1/2 - t1/2
    addOutput(createOutput<as_PJ301MPortGold>(Vec(220, 50+out_offset*1), module, BPMCalc::MS_OUTPUT + 2));
    addOutput(createOutput<as_PJ301MPortGold>(Vec(260, 50+out_offset*1), module, BPMCalc::MS_OUTPUT + 3));
    // ·1/4 - 1/4
    addOutput(createOutput<as_PJ301MPortGold>(Vec(220, 50+out_offset*2), module, BPMCalc::MS_OUTPUT + 4));
    addOutput(createOutput<as_PJ301MPortGold>(Vec(260, 50+out_offset*2), module, BPMCalc::MS_OUTPUT + 5));
    // t1/4 - ·1/8
    addOutput(createOutput<as_PJ301MPortGold>(Vec(220, 50+out_offset*3), module, BPMCalc::MS_OUTPUT + 6));
    addOutput(createOutput<as_PJ301MPortGold>(Vec(260, 50+out_offset*3), module, BPMCalc::MS_OUTPUT + 7));
    // 1/8 - t1/8
    addOutput(createOutput<as_PJ301MPortGold>(Vec(220, 50+out_offset*4), module, BPMCalc::MS_OUTPUT + 8));
    addOutput(createOutput<as_PJ301MPortGold>(Vec(260, 50+out_offset*4), module, BPMCalc::MS_OUTPUT + 9));
    // ·1/16 - 1/16
    addOutput(createOutput<as_PJ301MPortGold>(Vec(220, 50+out_offset*5), module, BPMCalc::MS_OUTPUT + 10));
    addOutput(createOutput<as_PJ301MPortGold>(Vec(260, 50+out_offset*5), module, BPMCalc::MS_OUTPUT + 11));

    // t1/16 - ·1/32
    addOutput(createOutput<as_PJ301MPortGold>(Vec(220, 50+out_offset*6), module, BPMCalc::MS_OUTPUT + 12));
    addOutput(createOutput<as_PJ301MPortGold>(Vec(260, 50+out_offset*6), module, BPMCalc::MS_OUTPUT + 13));
    
    // 1/32 - t1/32
    addOutput(createOutput<as_PJ301MPortGold>(Vec(220, 50+out_offset*7), module, BPMCalc::MS_OUTPUT + 14));
    addOutput(createOutput<as_PJ301MPortGold>(Vec(260, 50+out_offset*7), module, BPMCalc::MS_OUTPUT + 15));
    

  }
};


Model *modelBPMCalc = createModel<BPMCalc, BPMCalcWidget>("BPMCalc");
