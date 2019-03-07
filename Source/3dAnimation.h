#ifndef __3D_ANIMATION__
#define __3D_ANIMATION__

#include "GraphicStructures.h"
#include "Types.h"

class AnimSet
{
private:
    friend class AnimController;
    struct Output
    {
        std::string   name;
        FloatVec      scaleTime;
        VectorVec     scaleValue;
        FloatVec      rotationTime;
        QuaternionVec rotationValue;
        FloatVec      translationTime;
        VectorVec     translationValue;
    };
    typedef std::vector<Output> OutputVec;

    std::string animName;
    float       durationTicks;
    float       ticksPerSecond;
    OutputVec   outputs;

public:
    void SetData( const char* name, float ticks, float tps )
    {
        animName = name;
        durationTicks = ticks;
        ticksPerSecond = tps;
    }

    void AddOutput( const char* name, const FloatVec& st, const VectorVec& sv,
                    const FloatVec& rt, const QuaternionVec& rv, const FloatVec& tt, const VectorVec& tv  )
    {
        outputs.push_back( Output() );
        Output& o = outputs.back();
        o.name = name;
        o.scaleTime = st;
        o.scaleValue = sv;
        o.rotationTime = rt;
        o.rotationValue = rv;
        o.translationTime = tt;
        o.translationValue = tv;
    }

    const char* GetName()
    {
        return animName.c_str();
    }

    uint GetOutputCount()
    {
        return outputs.size();
    }

    float GetDuration()
    {
        return durationTicks / ticksPerSecond;
    }
};
typedef std::vector<AnimSet*> AnimSetVec;

class AnimController
{
private:
    struct Output
    {
        std::string   name;
        Matrix*       matrix;
        // Data for tracks blending
        BoolVec       valid;
        FloatVec      factor;
        VectorVec     scale;
        QuaternionVec rotation;
        VectorVec     translation;
    };
    typedef std::vector<Output>  OutputVec;
    typedef std::vector<Output*> OutputPtrVec;
    struct Track
    {
        struct Event
        {
            enum EType { Enable, Speed, Weight };
            Event( EType t, float v, float start, float smooth ) : type( t ), valueFrom( -1.0f ), valueTo( v ), startTime( start ), smoothTime( smooth ) {}
            EType type;
            float valueFrom;
            float valueTo;
            float startTime;
            float smoothTime;
        };
        typedef std::vector<Event> EventVec;

        bool         enabled;
        float        speed;
        float        weight;
        float        position;
        AnimSet*     anim;
        OutputPtrVec animOutput;
        EventVec     events;
    };
    typedef std::vector<Track> TrackVec;

    bool        cloned;
    AnimSetVec* sets;
    OutputVec*  outputs;
    TrackVec    tracks;
    float       curTime;

public:
    AnimController();
    ~AnimController();

    static AnimController* Create( uint track_count );

    AnimController* Clone();
    void            RegisterAnimationOutput( const char* frame_name, Matrix& output_matrix );
    void            RegisterAnimationSet( AnimSet* animation );
    AnimSet*        GetAnimationSet( uint index );
    AnimSet*        GetAnimationSetByName( const char* name );
    float           GetTrackPosition( uint track );
    uint            GetMaxNumAnimationSets();

    void SetTrackAnimationSet( uint track, AnimSet* anim );

    void Reset();

    float GetTime();

    void AddEventEnable( uint track, bool enable, float start_time );
    void AddEventSpeed( uint track, float speed, float start_time, float smooth_time );
    void AddEventWeight( uint track, float weight, float start_time, float smooth_time );

    void SetTrackEnable( uint track, bool enable );
    void SetTrackPosition( uint track, float position );

    void AdvanceTime( float time );

private:
    template<class T>
    void FindSRTValue( float time, FloatVec& times, std::vector<T>& values, T& result )
    {
        for( uint n = 0, m = times.size(); n < m; n++ )
        {
            if( n + 1 < m )
            {
                if( time >= times[n] && time < times[n + 1] )
                {
                    result = values[n];
                    T&    value = values[n + 1];
                    float factor = (time - times[n]) / (times[n + 1] - times[n]);
                    Interpolate( result, value, factor );
                    return;
                }
            }
            else
            {
                result = values[n];
            }
        }
    }

    void Interpolate( Quaternion& q1, Quaternion& q2, float factor );
    void Interpolate( Vector& v1, Vector& v2, float factor );
};

#endif // __3D_ANIMATION__
