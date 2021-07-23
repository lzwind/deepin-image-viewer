/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     ZhangYong <zhangyong@uniontech.com>
 *
 * Maintainer: ZhangYong <ZhangYong@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "imgoperate.h"

#include <QDebug>
#include <QDir>
#include <QMutex>
#include <QStandardPaths>
#include <QDirIterator>
#include <QThread>
#include <QImage>
#include <QImageReader>

#include "unionimage.h"
#include "pluginbaseutils.h"
#include "imageengine.h"

ImgOperate::ImgOperate(QObject *parent)
{
    Q_UNUSED(parent);
}

ImgOperate::~ImgOperate()
{

}

void ImgOperate::slotMakeImgThumbnail(QString thumbnailSavePath, QStringList paths, int makeCount)
{
    QString path;
    QImage tImg;
    imageViewerSpace::ItemInfo itemInfo;
    for (int i = 0; i < paths.size(); i++) {
        //达到制作数量则停止
        if (i == makeCount) {
            break;
        }
        path = paths.at(i);

        //获取图片类型
        itemInfo.imageType = getImageType(path);
        //获取路径类型
        itemInfo.pathType = getPathType(path);
        //获取原图分辨率
        QImageReader imagreader(path);
        itemInfo.imgOriginalWidth = imagreader.size().width();
        itemInfo.imgOriginalHeight = imagreader.size().height();

        //缩略图保存路径
        QString savePath = thumbnailSavePath + path;
        //保存为jpg格式
        savePath = savePath.mid(0, savePath.lastIndexOf('.')) + ImageEngine::instance()->makeMD5(savePath) + ".png";
        QFileInfo file(savePath);
        //缩略图已存在，执行下一个路径
        if (file.exists()) {
            tImg = QImage(savePath);
            itemInfo.image = tImg;
            emit sigOneImgReady(path, itemInfo);
            continue;
        }
        QString errMsg;
        QSize readSize;
//        if (!UnionImage_NameSpace::loadStaticImageFromFile(path, tImg, readSize, errMsg)) {
//            qDebug() << errMsg;
//            continue;
//        }
        if (!UnionImage_NameSpace::loadStaticImageFromFile(path, tImg, errMsg)) {
            qDebug() << errMsg;
            continue;
        }
        if (0 != tImg.height() && 0 != tImg.width() && (tImg.height() / tImg.width()) < 10 && (tImg.width() / tImg.height()) < 10) {
            bool cache_exist = false;
            if (tImg.height() != 200 && tImg.width() != 200) {
                if (tImg.height() >= tImg.width()) {
                    cache_exist = true;
                    tImg = tImg.scaledToWidth(200,  Qt::FastTransformation);
                } else if (tImg.height() <= tImg.width()) {
                    cache_exist = true;
                    tImg = tImg.scaledToHeight(200,  Qt::FastTransformation);
                }
            }
            if (!cache_exist) {
                if (static_cast<float>(tImg.height()) / static_cast<float>(tImg.width()) > 3) {
                    tImg = tImg.scaledToWidth(200,  Qt::FastTransformation);
                } else {
                    tImg = tImg.scaledToHeight(200,  Qt::FastTransformation);
                }
            }
        }
        //创建路径
        pluginUtils::base::mkMutiDir(savePath.mid(0, savePath.lastIndexOf('/')));
        if (tImg.save(savePath)) {
            itemInfo.image = tImg;
            emit sigOneImgReady(path, itemInfo);
        }
    }
}

imageViewerSpace::ImageType ImgOperate::getImageType(const QString &imagepath)
{
    imageViewerSpace::ImageType type = imageViewerSpace::ImageType::ImageTypeBlank;
    //todo
    return type;
}

imageViewerSpace::PathType ImgOperate::getPathType(const QString &imagepath)
{
    imageViewerSpace::PathType type = imageViewerSpace::PathType::PathTypeBlank;
    //todo
    return type;
}
