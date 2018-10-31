#ifndef __LOG_FILE__
#define __LOG_FILE__

class FileLogger
{
private:
    FILE* logFile;
    uint  startTick;

public:
    FileLogger( const char* fname );
    ~FileLogger();
    void Write( const char* fmt, ... );
};

#endif // __LOG_FILE__ //
