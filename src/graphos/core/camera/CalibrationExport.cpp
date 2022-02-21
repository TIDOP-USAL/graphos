/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "calibio.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include <fstream>

namespace graphos
{

CalibrationIO::CalibrationIO()
{
}


/*----------------------------------------------------------------*/

class OpenCVCalib
	: public CalibrationIO
{

public:

	OpenCVCalib()
	{
	}

	~OpenCVCalib()
	{
	}

// Heredado vía CalibrationIO

public:	

	bool write(std::shared_ptr<tl::Calibration> &calibration,
						 const QString &file) override
	{

		QFileInfo file_info(file);
		QDir parent_path(file_info.absolutePath());

		if (!parent_path.exists()) {
			if (!parent_path.mkpath(".")) {
				std::string err = "The output directory cannot be created: ";
				err.append(file_info.absolutePath().toStdString());
				throw std::runtime_error(err);
			}
		}

		std::ofstream stream(file.toStdString(), std::ios::trunc);
		if (stream.is_open()) {
			stream << "<?xml version=\"1.0\"?>\n";
			stream << "<opencv_storage>\n";
			stream << "<calibration_Time></calibration_Time>\n";
			stream << "<image_Width>" << camera.width() << "</image_Width>\n";
			stream << "<image_Height>" << camera.height() << "</image_Height>\n";
			stream << "<Camera_Matrix type_id=\"opencv-matrix\">\n";
			stream << "  <rows>3</rows>\n";
			stream << "  <cols>3</cols>\n";
			stream << "  <dt>d</dt>\n";
			stream << "  <data>\n";
			stream << "    " << QString::number(calibration->parameter(tl::Calibration::Parameters::focal), 'e', 8).toStdString() << " 0. " << QString::number(calibration->parameter(tl::Calibration::Parameters::cx), 'e', 8).toStdString() << " 0. \n";
			stream << "    " << QString::number(calibration->parameter(tl::Calibration::Parameters::focal), 'e', 8).toStdString() << " " << QString::number(calibration->parameter(tl::Calibration::Parameters::cy), 'e', 8).toStdString() << " 0. 0. 1.\n";
			stream << "  </data>\n";
			stream << "</Camera_Matrix>\n";
			stream << "<Distortion_Coefficients type_id=\"opencv-matrix\">\n";
			stream << "  <rows>5</rows>\n";
			stream << "  <cols>1</cols>\n";
			stream << "  <dt>d</dt>\n";
			stream << "  <data>\n";
			stream << "    " << (calibration->existParameter(tl::Calibration::Parameters::k1) ? QString::number(calibration->parameter(tl::Calibration::Parameters::k1)).toStdString() : "0.0") << " "
				<< (calibration->existParameter(tl::Calibration::Parameters::k2) ? QString::number(calibration->parameter(tl::Calibration::Parameters::k2), 'e', 8).toStdString() : "0.0") << " "
				<< (calibration->existParameter(tl::Calibration::Parameters::p1) ? QString::number(calibration->parameter(tl::Calibration::Parameters::p1), 'e', 8).toStdString() : "0.0") << " "
				<< (calibration->existParameter(tl::Calibration::Parameters::p2) ? QString::number(calibration->parameter(tl::Calibration::Parameters::p2), 'e', 8).toStdString() : "0.0") << " \n    "
				<< (calibration->existParameter(tl::Calibration::Parameters::k2) ? QString::number(calibration->parameter(tl::Calibration::Parameters::k2), 'e', 8).toStdString() : "0.0") << "</data>";
			stream << "</Distortion_Coefficients>\n";
			stream << "</opencv_storage>\n";

			stream.close();

		} else
			return false;

		return true;
	}

	std::shared_ptr<tl::Calibration> read(const QString &file) override
	{
		//QFile input(file);
		//if (input.open(QIODevice::ReadOnly)) {

		//	QXmlStreamReader stream;
		//	stream.setDevice(&input);

		//	if (stream.readNextStartElement()) {

		//		if (stream.name() == "opencv_storage") {

		//			while (stream.readNextStartElement()) {
		//				if (stream.name() == "image_Width") {
		//					mCameraCache[mActiveCameraId].setWidth(stream.readElementText().toInt());
		//				} else if (stream.name() == "image_Height") {
		//					mCameraCache[mActiveCameraId].setHeight(stream.readElementText().toInt());
		//				} else if (stream.name() == "Camera_Matrix") {

		//					while (stream.readNextStartElement()) {
		//						if (stream.name() == "rows") {
		//							QString rows = stream.readElementText();
		//						} else if (stream.name() == "cols") {
		//							QString cols = stream.readElementText();
		//						} else if (stream.name() == "data") {

		//							QString text = stream.readElementText();
		//							QStringList list = text.trimmed().split(" ", QString::SkipEmptyParts);

		//							std::shared_ptr<tl::Calibration> calibration = mCameraCache[mActiveCameraId].calibration();
		//							if (!calibration) {
		//								calibration = tl::CalibrationFactory::create(mCameraCache[mActiveCameraId].type());
		//								mCameraCache[mActiveCameraId].setCalibration(calibration);
		//							}
		//							calibration->setParameter(tl::Calibration::Parameters::focal, list[0].toDouble());
		//							calibration->setParameter(tl::Calibration::Parameters::focalx, list[0].toDouble());
		//							calibration->setParameter(tl::Calibration::Parameters::focaly, list[4].toDouble());
		//							calibration->setParameter(tl::Calibration::Parameters::cx, list[2].toDouble());
		//							calibration->setParameter(tl::Calibration::Parameters::cy, list[5].toDouble());

		//						} else
		//							stream.skipCurrentElement();
		//					}

		//				} else if (stream.name() == "Distortion_Coefficients") {

		//					while (stream.readNextStartElement()) {
		//						if (stream.name() == "rows") {
		//							QString rows = stream.readElementText();
		//						} else if (stream.name() == "cols") {
		//							QString cols = stream.readElementText();
		//						} else if (stream.name() == "data") {


		//							QString text = stream.readElementText();
		//							QStringList list = text.trimmed().split(" ", QString::SkipEmptyParts);

		//							std::shared_ptr<tl::Calibration> calibration = mCameraCache[mActiveCameraId].calibration();
		//							calibration->setParameter(tl::Calibration::Parameters::k1, list[0].toDouble());
		//							calibration->setParameter(tl::Calibration::Parameters::k2, list[1].toDouble());
		//							calibration->setParameter(tl::Calibration::Parameters::p1, list[2].toDouble());
		//							calibration->setParameter(tl::Calibration::Parameters::p2, list[3].toDouble());

		//						} else
		//							stream.skipCurrentElement();
		//					}

		//				} else
		//					stream.skipCurrentElement();
		//			}

		//		} else {
		//			//stream.raiseError(QObject::tr("Incorrect OpenCV Calibration file"));
		//			msgError("Incorrect OpenCV Calibration file");
		//		}

		//	} else {
		//		//stream.raiseError(QObject::tr("Incorrect OpenCV Calibration file"));
		//		msgError("Incorrect OpenCV Calibration file");
		//	}

		//	input.close();
		//}

		return std::shared_ptr<tl::Calibration>();
	}

};

/*----------------------------------------------------------------*/


class AgisoftCalib
	: public CalibrationIO
{

public:

	AgisoftCalib()
	{
	}

	~AgisoftCalib()
	{
	}

// Heredado vía CalibrationIO

public:

	bool write(std::shared_ptr<tl::Calibration> &calibration, 
						 const QString &file) override
	{
		return false;
	}

	std::shared_ptr<tl::Calibration> read(const QString &file) override
	{
		//std::shared_ptr<tl::Calibration> calibration = mCameraCache[mActiveCameraId].calibration();

		//QFile input(file);
		//if (input.open(QIODevice::ReadOnly)) {

		//	QXmlStreamReader stream;
		//	stream.setDevice(&input);

		//	if (stream.readNextStartElement()) {

		//		if (stream.name() == "calibration") {

		//			
		//			if (!calibration) {
		//				calibration = tl::CalibrationFactory::create(mCameraCache[mActiveCameraId].type());
		//				mCameraCache[mActiveCameraId].setCalibration(calibration);
		//			}

		//			while (stream.readNextStartElement()) {

		//				if (stream.name() == "projection") {
		//					TL_TODO("Comprobar tipo de cámara")
		//				} else if (stream.name() == "width") {
		//						mCameraCache[mActiveCameraId].setWidth(stream.readElementText().toInt());
		//					} else if (stream.name() == "height") {
		//						mCameraCache[mActiveCameraId].setHeight(stream.readElementText().toInt());
		//					} else if (stream.name() == "f") {
		//						calibration->setParameter(tl::Calibration::Parameters::focal, stream.readElementText().toDouble());
		//						calibration->setParameter(tl::Calibration::Parameters::focalx, stream.readElementText().toDouble());
		//						calibration->setParameter(tl::Calibration::Parameters::focaly, stream.readElementText().toDouble());
		//					} else if (stream.name() == "cx") {
		//						calibration->setParameter(tl::Calibration::Parameters::cx, stream.readElementText().toDouble());
		//					} else if (stream.name() == "cy") {
		//						calibration->setParameter(tl::Calibration::Parameters::cy, stream.readElementText().toDouble());
		//					} else if (stream.name() == "k1") {
		//						calibration->setParameter(tl::Calibration::Parameters::k1, stream.readElementText().toDouble());
		//					} else if (stream.name() == "k2") {
		//						calibration->setParameter(tl::Calibration::Parameters::k2, stream.readElementText().toDouble());
		//					} else if (stream.name() == "k3") {
		//						calibration->setParameter(tl::Calibration::Parameters::k3, stream.readElementText().toDouble());
		//					} else if (stream.name() == "p1") {
		//						calibration->setParameter(tl::Calibration::Parameters::p1, stream.readElementText().toDouble());
		//					} else if (stream.name() == "p2") {
		//						calibration->setParameter(tl::Calibration::Parameters::p2, stream.readElementText().toDouble());
		//					} else if (stream.name() == "date") {
		//					} else
		//						stream.skipCurrentElement();
		//			}

		//		} else {
		//			//stream.raiseError(QObject::tr("Incorrect Agisoft Calibration file"));
		//			msgError("Incorrect Agisoft Calibration file");
		//		}

		//	} else {
		//		//stream.raiseError(QObject::tr("Incorrect Agisoft Calibration file"));
		//		msgError("Incorrect Agisoft Calibration file");
		//	}

		//	input.close();
		//}
		return std::shared_ptr<tl::Calibration>();
	}

};


/*----------------------------------------------------------------*/


class Pix4DCalib
	: public CalibrationIO
{

public:

	Pix4DCalib()
	{
	}

	~Pix4DCalib()
	{
	}

// Heredado vía CalibrationIO

  bool write(std::shared_ptr<tl::Calibration> &calibration, const QString &file) override
	{
		return false;
	}

	std::shared_ptr<tl::Calibration> read(const QString &file) override
	{
		return std::shared_ptr<tl::Calibration>();
	}

};


/*----------------------------------------------------------------*/



std::unique_ptr<CalibrationIO> CalibrationIOFactory::create(const QString &format)
{
	std::unique_ptr<CalibrationIO> calibration_io;

	if (format == QString("OpenCV")) {
		calibration_io = std::make_unique<OpenCVCalib>();
	} else if (format == QString("Agisoft")) {
		calibration_io = std::make_unique<AgisoftCalib>();
	} else if (format == QString("Pix4D")) {
		calibration_io = std::make_unique<AgisoftCalib>();
	} else {
		throw std::runtime_error("Invalid Calibration format");
	}

	return calibration_io;
}

} // namespace graphos
