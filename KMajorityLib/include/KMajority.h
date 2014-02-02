/*
 * KMajority.h
 *
 *  Created on: Aug 28, 2013
 *      Author: andresf
 */

#ifndef KMAJORITY_INDEX_H_
#define KMAJORITY_INDEX_H_

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>

#include <DynamicMat.hpp>

typedef cvflann::Hamming<uchar> Distance;
typedef typename Distance::ResultType DistanceType;

namespace vlr {

// Allowed nearest neighbor index algorithms
enum indexType {
	LINEAR = 0, HIERARCHICAL = 1
};

cvflann::NNIndex<Distance>* createIndexByType(
		const cvflann::Matrix<typename Distance::ElementType>& dataset,
		const Distance& distance, vlr::indexType type);

class KMajority {

protected:

	// Number of clusters
	int m_numClusters;
	// Maximum number of iterations
	int m_maxIterations;
	// Method for initializing centers
	cvflann::flann_centers_init_t m_centersInitMethod;
	// Reference to the matrix with data to cluster
	vlr::Mat& m_dataset;
	// Dimensionality of the data under clustering (in Bytes)
	int m_dim;
	// Number of data instances
	int m_numDatapoints;
	// List of the cluster each data point belongs to
	std::vector<int> m_belongsTo;
	// List of distance from each data point to the cluster it belongs to
	std::vector<DistanceType> m_distanceTo;
	// Number of data points assigned to each cluster
	std::vector<int> m_clusterCounts;
	// Matrix of clusters centers
	cv::Mat m_centroids;
	vlr::indexType m_nnMethod;
	// Index for addressing nearest neighbors search
	cvflann::NNIndex<Distance>* m_nnIndex = NULL;

public:

	/**
	 * Class constructor.
	 *
	 * @param numClusters
	 * @param maxIterations
	 * @param data
	 * @param indices - The set of indices indicating the data points to cluster
	 * @param indicesLength
	 * @param belongsTo
	 * @param centersInit
	 */
	KMajority(int numClusters = 0, int maxIterations = 0, vlr::Mat& data =
			vlr::DEFAULT_INPUTDATA, vlr::indexType nnMethod =
			vlr::indexType::LINEAR,
			cvflann::flann_centers_init_t centersInitMethod =
					cvflann::FLANN_CENTERS_RANDOM);

	/**
	 * Class destroyer.
	 */
	~KMajority();

	/**
	 * Implements k-means clustering loop.
	 */
	void cluster();

	/**
	 * Saves the vocabulary to a file stream.
	 *
	 * @param filename - The name of the file stream where to save the vocabulary
	 */
	void save(const std::string& filename) const;

	/**
	 * Loads the vocabulary to a file stream.
	 *
	 * @param filename - The name of the file stream where to save the vocabulary
	 */
	void load(const std::string& filename);

	/**
	 * Decomposes data into bits and accumulates them into cumResult.
	 *
	 * @param data - Row vector containing the data to accumulate
	 * @param accVector - Row oriented accumulator vector
	 */
	static void cumBitSum(const cv::Mat& data, cv::Mat& accVector);

	/**
	 * Component wise thresholding of accumulator vector.
	 *
	 * @param accVector - Row oriented accumulator vector
	 * @param result - Row vector containing the thresholding result
	 * @param threshold - Threshold value, typically the number of data points used to compute the accumulator vector
	 */
	static void majorityVoting(const cv::Mat& accVector, cv::Mat& result,
			const int& threshold);

	/**** Getters ****/

	const cv::Mat& getCentroids() const;

	const std::vector<int>& getClusterCounts() const;

	const std::vector<int>& getClusterAssignments() const;

private:

	/**
	 * Initializes cluster centers choosing among the data points indicated by indices.
	 */
	void initCentroids();

	/**
	 * Implements majority voting scheme for cluster centers computation
	 * based on component wise majority of bits from data matrix
	 * as proposed by Grana2013.
	 */
	void computeCentroids();

	/**
	 * Assigns data to clusters by means of Hamming distance.
	 *
	 * @return true if convergence was achieved (cluster assignment didn't changed), false otherwise
	 */
	bool quantize();

	/**
	 * Fills empty clusters using data assigned to the most populated ones.
	 */
	void handleEmptyClusters();

};

} /* namespace vlr */

#endif /* KMAJORITY_H_ */
