#pragma once
#include "Util.h"
#include "RenderData.h"
#include "ScreenData.hpp"
class RenderManager
{
public:
	typedef void(*VertexShaderCallBack)(const RenderData *, RenderData *);
	typedef void(*PixelShaderCallBack)(const RenderData *, RenderMath::Color *);
	typedef std::vector<int> IndexBuffer;
	typedef Matrix<double> DepthBuffer;
	typedef Matrix<Pixel> ScreenBuffer;
	enum PlaneDir
	{
		PD_UP, PD_DOWN, PD_RIGHT, PD_LEFT, PD_FAR, PD_NEAR
	};


	static RenderManager * const GetInstance();
	~RenderManager();

	void ResizeClient(const int width, const int height);

	// ==================== vertex function ==================== 
	void SetInVertexBuffer(const void * const pSource, 
						   const int vertexNum, 
						   size_t stride = sizeof(Vertex));
	void SetIndexBuffer(const void * const pSource, 
						const int indexNum);
	void SetVertexShader(VertexShaderCallBack, size_t outVertexStride);
	void DoVertexShader();

	// ==================== set camera parameter ====================
	void SetCamera(const double inZoomX, 
				   const double inRadio, 
				   const double inNear, 
				   const double inFar);

	// ==================== render pipeline ====================
	void BeginRender();

	// ==================== clip & cull ====================
	void DoClipping();
	// ClipPolygon return 1 if vertexes are enough for a polygon
	int ClipPolygon(IndexBuffer * polyIndexBuffer, 
					const PlaneDir planeDir);
	// ClipEdge return 1 if there is new vertex
	int ClipEdge(const int indexStart, 
				 const int indexEnd, 
				 const PlaneDir planeDir, 
				 IndexBuffer * outBuffer);
	double GetIntersectK(const RenderMath::Vector4 & posStart, 
						 const RenderMath::Vector4 & posEnd, 
						 const PlaneDir planeDir);
	void Triangulation(const IndexBuffer * inBuffer);

	// ==================== project ====================
	void DoProjection();

	// ==================== rasterize ====================
	void DoRasterization();
	void UpperTriangleRaster(const Buffer & triangleBuffer);
	void BottomTriangleRaster(const Buffer & triangleBuffer);
	void GetEdgeVertex(const Vertex * pTop,
					   const Vertex * pLeft,
					   const Vertex * pRight,
					   const double y,
					   Buffer * outBuffer);
	void HorizontalRaster(const Vertex * pTop,
						  const Buffer & edgeVertexBuffer,
						  const double leftSlope,
						  const double rightSlope);

	// ==================== pixel shader ====================
	void SetPixelShader(PixelShaderCallBack);
	void DoPixelShader();
	void InilScreen();

	// ==================== clear buffer ====================
	void ClearBuffer(Buffer & buffer);
	void ClearBuffer(IndexBuffer & buffer);

	// ==================== vertex data ====================
	Buffer inVertexBuffer;
	Buffer outVertexBuffer;
	IndexBuffer indexBuffer;
	size_t outVertexStride;
	VertexShaderCallBack vertexShader;

	// ==================== pixel data ====================
	Buffer PixelBuffer;
	PixelShaderCallBack pixelShader;

	// ==================== client parameters ====================
	int width, height;
	Pixel backgroundColor;
	DepthBuffer depthBuffer;
	ScreenBuffer screenBuffer;
	// ==================== camera parameters ====================
	double zoomX, zoomY, nearDis, farDis;
	RenderMath::Matrix4X4 clipMatrix;
	IndexBuffer clippedIndexBuffer;
	Plane clipPlanes[6];
private:
	// ==================== invalid pubilc constructor ====================
	RenderManager();
	RenderManager(const RenderManager &);
	RenderManager & operator=(const RenderManager &);
};

