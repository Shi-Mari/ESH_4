#include <stdint.h>


#pragma pack(push, 1) // установка фрагментации структур по 1 байту

enum EMsgType
{
	NAVIG_GET,
	CUR_CMD_GET,
	IS_ALIVE,
	SENSOR_DATA_GET,
	SENSOR_LIST_GET,

	POINTS_MOVE,
	POINTS_ADD,
	POINTS_CLEAR, 	// удаление всех точек после целевой
	SIGNAL_SET,
	VELOCITY_SET,
	CONTROL_SET, 	// переключение между режимами: автономный/телеуправление

	PID_SET,
	STEP_SET,
	TIME_SET,
	MODE_SET,
	TYPE_CMP,
};

enum EMovementMode
{
	POINTS, // посещение каждой точки
	LINEAR, // движение по ломаной
	SPLINE  // движение по сплайну
};

enum EEventType
{
	MISSION_COMPLETE,
	ERROR
};

struct SNavigGet
{
	float x;
	float y;
	float vx;
	float vy;
	float ax;
	float ay;
};

struct SSensorData
{
	uint8_t sensorId;
	uint16_t dataSize;
	uint8_t data[1020];
};

struct SSensor
{
	uint8_t id;
	uint8_t type;
};

struct SSensorsList
{
	uint8_t sensorNum;
	SSensor sensors[255];
};

struct SPoint3D
{
	float x;
	float y;
	float z;
};

struct SPointsMove
{
	SPoint3D pts[20];
	uint8_t ptsNum;
	float velocity;
	EMovementMode mode;
};

struct SPointsAdd
{
	SPoint3D pts[20];
	uint8_t ptsNum;
};

struct SSignalSet
{
	uint8_t outputIndex;
	bool val;
};

struct SVelocitySet
{
	float velocity;
};

struct SControlSet
{
	uint8_t control; // 0 == auto
};

struct SPidSet
{
	float p, i, d;
};

struct SStepSet
{
	float step;
};

struct STimeSet
{
	uint64_t time; // millis
};

struct SModeSet
{
	uint8_t mode;
};

struct SAddress
{
	uint8_t subsyst;
	uint8_t NODE;
	uint8_t cmp;
};
#pragma pack(pop)
