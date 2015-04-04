#include <QtpComp.h>
#include <QtpPin.h>

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>

#include <QtWidgets>

QtpComp::QtpComp(CompInfo const& compInfo, uint id, QPointF const& position, QGraphicsItem* parent)
    : QGraphicsPolygonItem(parent)
{
    setPos(position);

    _id = id;
    _color = Qt::black;
    _compInfo = compInfo;
    _contextMenu = new QMenu();

    _polygon << QPointF(-15, -15) << QPointF(15, -15) << QPointF(15, 15) << QPointF(-15, 15) << QPointF(-15, -15);

    _nameText = new QGraphicsTextItem(this);

    setName(compInfo.typeName);
    foreach (QString const& pin, compInfo.inPins)
    {
        addInPin(pin);
    }
    foreach (QString const& pin, compInfo.outPins)
    {
        addOutPin(pin);
    }

    setPolygon(_polygon);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    //------------------------
    _contextMenu->addSeparator();

//    Null,
//    Bool,
//    Int,
//    Float,
//    String,
//    FilePath,  // this is essentially just a string, but helps when determining an appropriate user input method
//    List,    // this type acts as a vector (available items), an int (index selected), and a string (item selected)
//    Trigger  // this type has no value, SetParam(triggerParam) simply represents a trigger. E.g. a button press

    // Bool
    {
        QCheckBox* custom = new QCheckBox(_contextMenu);
        custom ->setText("Enable");
        QWidgetAction* customAction = new QWidgetAction(_contextMenu);
        customAction->setDefaultWidget(custom);
        _contextMenu->addAction(customAction);
    }

    // Int
    // Float
    {
        QWidget* intSlider = new QWidget(_contextMenu);

        QSlider* slider = new QSlider(Qt::Horizontal, intSlider);
        slider ->setRange(0, 1000);

        QLabel* label = new QLabel(intSlider);
        label->setNum(slider->sliderPosition());

        QObject::connect(slider, SIGNAL(valueChanged(int)), label, SLOT(setNum(int)));

        QHBoxLayout* layout = new QHBoxLayout(intSlider);
        layout->addWidget(slider);
        layout->addWidget(label);

        QWidgetAction* intSliderAction = new QWidgetAction(_contextMenu);
        intSliderAction->setDefaultWidget(intSlider);
        _contextMenu->addAction(intSliderAction);
    }

    // String
    {
        QLineEdit* custom = new QLineEdit(_contextMenu);
        custom ->setText("Enable");
        QWidgetAction* customAction = new QWidgetAction(_contextMenu);
        customAction->setDefaultWidget(custom);
        _contextMenu->addAction(customAction);
    }

    // FilePath
    {
        QWidget* fileBrowser = new QWidget(_contextMenu);

        QLineEdit* edtPath = new QLineEdit(fileBrowser);
        edtPath->setEnabled(false);

        QPushButton* btnBrowse = new QPushButton(fileBrowser);
        btnBrowse->setText("Browse");

        QHBoxLayout* layout = new QHBoxLayout(fileBrowser);
        layout->addWidget(edtPath);
        layout->addWidget(btnBrowse);

        QWidgetAction* fileBrowserAction = new QWidgetAction(_contextMenu);
        fileBrowserAction->setDefaultWidget(fileBrowser);
        _contextMenu->addAction(fileBrowserAction);

//        QFileDialog* custom = new QFileDialog(_contextMenu);
//        QWidgetAction* customAction = new QWidgetAction(_contextMenu);
//        customAction->setDefaultWidget(custom);
//        _contextMenu->addAction(customAction);
    }

    // List
    {
        QComboBox* custom = new QComboBox(_contextMenu);
        QWidgetAction* customAction = new QWidgetAction(_contextMenu);
        customAction->setDefaultWidget(custom);
        _contextMenu->addAction(customAction);
    }

    // Trigger
    {
        QPushButton* custom = new QPushButton(_contextMenu);
        custom->setText("Play");
        QWidgetAction* customAction = new QWidgetAction(_contextMenu);
        customAction->setDefaultWidget(custom);
        _contextMenu->addAction(customAction);
    }
    //------------------------
}

QtpComp::~QtpComp()
{
    delete _contextMenu;
    removePins();
}

uint QtpComp::id()
{
    return _id;
}

int QtpComp::type() const
{
    return Type;
}

void QtpComp::setColor(const QColor& color)
{
    _color = color;
    setBrush(_color);

    foreach (QtpPin* pin, _inPins)
    {
        pin->setBrush(_color);
    }

    foreach (QtpPin* pin, _outPins)
    {
        pin->setBrush(_color);
    }
}

void QtpComp::addInPin(QString pinName)
{
    QtpPin* pin = new QtpPin(QtpPin::InPin, pinName, _inPins.size(), this);
    pin->setPos(-21, 20 * _inPins.size());
    pin->setBrush(_color);
    _inPins.append(pin);

    updatePolygon();
}

void QtpComp::addOutPin(QString pinName)
{
    QtpPin* pin = new QtpPin(QtpPin::OutPin, pinName, _outPins.size(), this);
    pin->setPos(21, 20 * _outPins.size());
    pin->setBrush(_color);
    _outPins.append(pin);

    updatePolygon();
}

void QtpComp::removePins()
{
    foreach (QtpPin* pin, _inPins)
    {
        delete pin;
    }

    _inPins.clear();

    foreach (QtpPin* pin, _outPins)
    {
        delete pin;
    }

    _outPins.clear();

    updatePolygon();
}

QPixmap QtpComp::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 1));

    int pinsAfterFirst = std::max(_inPins.size(), _outPins.size());
    pinsAfterFirst = --pinsAfterFirst < 0 ? 0 : pinsAfterFirst;

    painter.translate(125, 125 - (10 * pinsAfterFirst));
    painter.drawPolyline(_polygon);

    painter.translate(-20, 0);
    foreach (QtpPin* pin, _inPins)
    {
        painter.drawPolyline(pin->polygon());
        painter.translate(0, 20);
    }

    painter.resetTransform();
    painter.translate(150, 125 - (10 * pinsAfterFirst));
    foreach (QtpPin* pin, _outPins)
    {
        painter.drawPolyline(pin->polygon());
        painter.translate(0, 20);
    }

    return pixmap;
}

QtpComp::CompInfo QtpComp::compInfo() const
{
    return _compInfo;
}

void QtpComp::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    scene()->clearSelection();
    setSelected(true);
    _contextMenu->exec(event->screenPos());
}

void QtpComp::setName(QString name)
{
    _nameText->setPlainText(name);
    _nameText->setPos(-_nameText->boundingRect().width() / 2, -39);
}

void QtpComp::updatePolygon()
{
    int pinsAfterFirst = std::max(_inPins.size(), _outPins.size());
    pinsAfterFirst = --pinsAfterFirst < 0 ? 0 : pinsAfterFirst;

    _polygon.replace(2, QPointF(15, 15 + (20 * pinsAfterFirst)));
    _polygon.replace(3, QPointF(-15, 15 + (20 * pinsAfterFirst)));
}
