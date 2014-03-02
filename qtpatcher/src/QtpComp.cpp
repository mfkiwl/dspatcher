#include <QtpComp.h>
#include <QtpPin.h>

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>

QtpComp::QtpComp( CompType compType, QMenu* contextMenu, QPointF const& position,
                    QGraphicsItem* parent )
  : QGraphicsPolygonItem( parent )
{
  setPos( position );

  _color = Qt::black;
  _compType = compType;
  _contextMenu = contextMenu;

  _polygon << QPointF( -15, -15 ) << QPointF( 15, -15 ) << QPointF( 15, 15 ) << QPointF( -15, 15 )
           << QPointF( -15, -15 );

  _nameText = new QGraphicsTextItem( this );

  setName( compType.name );
  foreach( QString const& pin, compType.inPins )
  {
    addInPin( pin );
  }
  foreach( QString const& pin, compType.outPins )
  {
    addOutPin( pin );
  }

//  switch( _compType )
//  {
//    case Process:
//    {
//      setName("Gain");
//      addInPin("Audio In");
//      addOutPin("Audio Out");
//    }
//    break;
//    case Process2:
//    {
//      setName("Audio Device");
//      addInPin("Left Spk");
//      addInPin("Right Spk");
//      addOutPin("Left Mic");
//      addOutPin("Right Mic");
//    }
//    break;
//    case Process3:
//    {
//      setName("Ambisonix");
//      addInPin("Audio In");
//      addOutPin("Spk LF");
//      addOutPin("Spk LR");
//      addOutPin("Spk RF");
//      addOutPin("Spk RR");
//    }
//    break;
//  }

  setPolygon( _polygon );

  setFlag( QGraphicsItem::ItemIsMovable, true );
  setFlag( QGraphicsItem::ItemIsSelectable, true );
}

QtpComp::~QtpComp()
{
  removePins();
}

int QtpComp::type() const
{
  return Type;
}

void QtpComp::setColor( const QColor& color )
{
  _color = color;
  setBrush( _color );

  foreach( QtpPin* pin, _inPins )
  {
    pin->setBrush( _color );
  }

  foreach( QtpPin* pin, _outPins )
  {
    pin->setBrush( _color );
  }
}

void QtpComp::addInPin( QString pinName )
{
  QtpPin* pin = new QtpPin( QtpPin::InPin, pinName, this );
  pin->setPos( -21, 20 * _inPins.size() );
  pin->setBrush( _color );
  _inPins.append( pin );

  updatePolygon();
}

void QtpComp::addOutPin( QString pinName )
{
  QtpPin* pin = new QtpPin( QtpPin::OutPin, pinName, this );
  pin->setPos( 21, 20 * _outPins.size() );
  pin->setBrush( _color );
  _outPins.append( pin );

  updatePolygon();
}

void QtpComp::removePins()
{
  foreach( QtpPin* pin, _inPins )
  {
    delete pin;
  }

  _inPins.clear();

  foreach( QtpPin* pin, _outPins )
  {
    delete pin;
  }

  _outPins.clear();

  updatePolygon();
}

QPixmap QtpComp::image() const
{
  QPixmap pixmap( 250, 250 );
  pixmap.fill( Qt::transparent );
  QPainter painter( &pixmap );
  painter.setPen( QPen( Qt::black, 1 ) );

  int pinsAfterFirst = std::max( _inPins.size(), _outPins.size() );
  pinsAfterFirst = --pinsAfterFirst < 0 ? 0 : pinsAfterFirst;

  painter.translate( 125, 125 - ( 10 * pinsAfterFirst ) );
  painter.drawPolyline( _polygon );

  painter.translate( -20, 0 );
  foreach( QtpPin* pin, _inPins )
  {
    painter.drawPolyline( pin->polygon() );
    painter.translate( 0, 20 );
  }

  painter.resetTransform();
  painter.translate( 150, 125 - ( 10 * pinsAfterFirst ) );
  foreach( QtpPin* pin, _outPins )
  {
    painter.drawPolyline( pin->polygon() );
    painter.translate( 0, 20 );
  }

  return pixmap;
}

QtpComp::CompType QtpComp::compType() const
{
  return _compType;
}

void QtpComp::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
  scene()->clearSelection();
  setSelected( true );
  _contextMenu->exec( event->screenPos() );
}

void QtpComp::setName( QString name )
{
  _nameText->setPlainText( name );
  _nameText->setPos( -_nameText->boundingRect().width() / 2, -39 );
}

void QtpComp::updatePolygon()
{
  int pinsAfterFirst = std::max( _inPins.size(), _outPins.size() );
  pinsAfterFirst = --pinsAfterFirst < 0 ? 0 : pinsAfterFirst;

  _polygon.replace( 2, QPointF( 15, 15 + ( 20 * pinsAfterFirst ) ) );
  _polygon.replace( 3, QPointF( -15, 15 + ( 20 * pinsAfterFirst ) ) );
}
