#ifndef REALTIMEPLOT_H
#define REALTIMEPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_directpainter.h>
#include <qwt_series_data.h>


class RealTimePlot : public QwtPlot
{
public:
    RealTimePlot();

private:

};

// ***************************************

class CurveData: public QwtArraySeriesData<QPointF>
{
public:
    CurveData()
    {
    }
    /* ПОКА НЕ НУЖНО
    virtual QRectF boundingRect() const
    {
        if (d_boundingRect.width() < 0.0)
            d_boundingRect = qwtBoundingRect(*this);

        return d_boundingRect;
    }
    */

    // ДОБАВЛЕНИЕ ТОЧКИ
    inline void appendDataPoint(const QPointF &point)
    {
        d_samples += point;
    }

    /* ПОКА НЕ НУЖНО
    void clear()
    {
        d_samples.clear();
        d_samples.squeeze();
        d_boundingRect = QRectF(0.0, 0.0, -1.0, -1.0);
    }
    */
};

#endif // REALTIMEPLOT_H
