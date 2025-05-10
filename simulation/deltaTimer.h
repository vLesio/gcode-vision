#ifndef DELTA_TIMER_H
#define DELTA_TIMER_H

class DeltaTimer {
private:
    double lastTime = 0.0;
    bool paused = false;
    double pauseTime = 0.0;

public:
    DeltaTimer();

    float tick();            
    void pause();
    void resume();
    bool isPaused() const;
    void reset();
};

#endif
