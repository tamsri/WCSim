#ifndef RAY_TRACER_H
#define RAY_TRACER_H

// Important Constants
#define LIGHT_SPEED 299792458.f
#define PI 3.141592
// IOR - Index of Refraction
// Relative permittivity according to ITU-R, P.2040-1. (for 1-100 GHz)
#define AIR_IOR 1.00029f
#define CONCRETE_IOR 5.31f

#include <vector>
#include <utility>
#include <cstdlib>


#include <glm/glm.hpp>

#include "record.hpp"

class Shader;
class PolygonMesh;
class Ray;
class Triangle;
class Shader;
class Object;
class Camera;
class Transmitter;
class Receiver;
class Recorder;

struct Record;
struct Point;
struct Result;

enum Polarization : bool {
	TM = true,
	TE = false
};

class RayTracer {
public:
	RayTracer(PolygonMesh * map);
	
	// Ray Tracing Part
	void Trace( glm::vec3 start_position,
                glm::vec3 end_position,
                std::vector<Record> & records) const;
    void TraceMap( glm::vec3 start_position,
                   glm::vec3 end_position,
                  std::vector<Record> &records) const;

	void LineTrace(  glm::vec3 start_position,
                     glm::vec3 end_position,
                     std::vector<Record> & records
    ) const;

    void ReflectTrace(glm::vec3 start_position,
                      glm::vec3 end_position,
                      std::vector<Record> &records) const;


	// Line of Sight
	bool IsDirectHit( glm::vec3 start_position, glm::vec3 end_position) const;
	
	// Reflection
	std::map<Triangle *, bool> ScanHit(glm::vec3 position) const;
	std::vector <Triangle*> ScanHitVec(glm::vec3 position) const;
	bool IsReflected(glm::vec3 start_position, glm::vec3 end_position, std::vector<glm::vec3> & reflected_points) const;
	static float CalculateReflectionCoefficient(glm::vec3 start_position, glm::vec3 end_position, glm::vec3 reflection_position, Polarization polar) ;
	static glm::vec3 ReflectedPointOnTriangle(const Triangle * triangle, glm::vec3 point) ;

	// Diffraction
	bool IsKnifeEdgeDiffraction(const glm::vec3 start_point, const glm::vec3 end_point, std::vector<glm::vec3> & edges_points) const;
	bool FindEdge(const glm::vec3 start_position, const glm::vec3 end_position, glm::vec3 & edge_position) const;
	static glm::vec3 NearestEdgeFromPoint(glm::vec3 point_position, std::vector<glm::vec3> & edges_points ) ;
	void CleanEdgePoints(const glm::vec3 start_position, const glm::vec3 end_position, std::vector<glm::vec3> & edges_points) const;
	float GetHighestPoint(std::vector<glm::vec3> edges) const;

	// Calculations
	float CalculateSingleKnifeEdge(glm::vec3 start_position, glm::vec3 edge_position, glm::vec3 end_position, float frequency) const;
	static float CalculateDiffractionByV(float v) ;
	static float CalculateVOfEdge(glm::vec3 start_position, glm::vec3 edge_position, glm::vec3 end_position, float frequency) ;
	static void CalculateCorrectionCosines(glm::vec3 start_position, std::vector<glm::vec3> edges, glm::vec3 end_position, std::pair<float, float> & calculated_cosines) ;

	void CalculateDirectPath(const Record & record, Result & result, Transmitter * transmitter,
                                     Receiver * receiver) const ;
    void CalculateReflections(const Record & record, Result & result, Transmitter * transmitter,
                              Receiver * receiver) const;
    void CalculateReflection( const glm::vec3 & tx_position, const glm::vec3 & rx_position,
                              const float & tx_freq, const float & tx_gain,
                              const float & rx_gain, const float & tx_power,
                              const glm::vec3 & ref_position, Result & result) const;

    void CalculateDiffraction(const Record & record, Result & result,
                              Transmitter * transmitter, Receiver * receiver) const;

    void GetDrawComponents( const glm::vec3 & start_position, const glm::vec3 &end_position,
                            std::vector<Record> & records, std::vector<Object *> & objects) const;
    bool CalculatePathLoss(Transmitter* transmitter, Receiver * receiver,
                           const std::vector<Record>& records,
                           Result& result) const;
    bool CalculatePathLossMap(Transmitter* transmitter, Receiver * receiver,
                              const std::vector<Record>& records,
                              Result& result) const;
private:
	PolygonMesh * map_;

};
#endif // !RAY_TRACER_H