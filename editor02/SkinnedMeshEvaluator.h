#ifndef AV_ANIMEVALUATOR_H_INCLUDED
#define AV_ANIMEVALUATOR_H_INCLUDED

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <vector>

class AnimEvaluator
{
public:
	/** Constructor on a given animation. The animation is fixed throughout the lifetime of
	* the object.
	* @param pAnim The animation to calculate poses for. Ownership of the animation object stays
	*   at the caller, the evaluator just keeps a reference to it as long as it persists.
	*/
	void SetNewAnimation(const aiAnimation* pAnim);
	AnimEvaluator(const aiAnimation* pAnim);

	/** Evaluates the animation tracks for a given time stamp. The calculated pose can be retrieved as a
	* array of transformation matrices afterwards by calling GetTransformations().
	* @param pTime The time for which you want to evaluate the animation, in seconds. Will be mapped into the animation cycle, so
	*   it can be an arbitrary value. Best use with ever-increasing time stamps.
	*/

	
	void Evaluate(double pTime);

	/** Returns the transform matrices calculated at the last Evaluate() call. The array matches the mChannels array of
	* the aiAnimation. */
	const std::vector<aiMatrix4x4>& GetTransformations() const { return mTransforms; }

protected:
	/** The animation we're working on */
	const aiAnimation* mAnim;
	struct PosData
	{
		PosData()
		{
			x = y = z = 0;
		}
		unsigned int x, y, z;
	};

	double mLastTime;
	std::vector<PosData> mLastPositions;

	/** The array to store the transformations results of the evaluation */
	std::vector<aiMatrix4x4> mTransforms;
};


#endif // AV_ANIMEVALUATOR_H_INCLUDED