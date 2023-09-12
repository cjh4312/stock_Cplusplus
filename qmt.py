# This Python file uses the following encoding: utf-8

# if __name__ == "__main__":
#     pass

import random
from xtquant.xttrader import XtQuantTrader
from xtquant.xttype import StockAccount
from xtquant import xtconstant

path = r'D:\Program Files\Finance\国金证券QMT交易端\userdata_mini'

def qmtBuy(account,code,numbers,price):
#    path = r'D:\Program Files\Finance\国金证券QMT交易端\userdata_mini'
    session_id = int(random.randint(100000, 999999))
    xt_trader = XtQuantTrader(path, session_id)

    xt_trader.start()
    connect_result = xt_trader.connect()
    acc = StockAccount(account)
    subscribe_result = xt_trader.subscribe(acc)

    order_id = xt_trader.order_stock(acc,code,xtconstant.STOCK_BUY,numbers,xtconstant.FIX_PRICE,price)
    return f'{connect_result};{subscribe_result};{order_id}'

def qmtSell(account,code,numbers,price):
#    path = r'D:\Program Files\Finance\国金证券QMT交易端\userdata_mini'
    session_id = int(random.randint(100000, 999999))
    xt_trader = XtQuantTrader(path, session_id)

    xt_trader.start()
    connect_result = xt_trader.connect()
    acc = StockAccount(account)
    subscribe_result = xt_trader.subscribe(acc)

    order_id = xt_trader.order_stock(acc,code,xtconstant.STOCK_SELL,numbers,xtconstant.FIX_PRICE,price)
    return f'{connect_result};{subscribe_result};{order_id}'

def getAsset(account):
#    path = r'D:\Program Files\Finance\国金证券QMT交易端\userdata_mini'
    session_id = int(random.randint(100000, 999999))
    xt_trader = XtQuantTrader(path, session_id)

    xt_trader.start()
    connect_result = xt_trader.connect()

    acc = StockAccount(account)
    subscribe_result = xt_trader.subscribe(acc)

    asset = xt_trader.query_stock_asset(acc)
    if asset:
        return format(asset.cash)
    else:
        return f'{connect_result};{subscribe_result}'

def getPositions(account):
#    path = r'D:\Program Files\Finance\国金证券QMT交易端\userdata_mini'
    session_id = int(random.randint(100000, 999999))
    xt_trader = XtQuantTrader(path, session_id)

    xt_trader.start()
    connect_result = xt_trader.connect()

    acc = StockAccount(account)
    subscribe_result = xt_trader.subscribe(acc)

    positions = xt_trader.query_stock_positions(acc)
    if len(positions)!=0:
        return f"{positions[-1].stock_code};{positions[-1].can_use_volume}"
    else:
        return f'{connect_result};{subscribe_result}'
#    positions = xt_trader.query_stock_positions(acc)
#    info=[]
#    if len(positions)!=0:
#        info.append({'证券代码':positions.stock_code})
#        info.append({'持仓数量':positions.volume})
#        info.append({'可用数量':positions.can_use_volume})
#        info.append({'平均持仓成本':positions.open_price})
#        print(info)
#    else:
#        print('无持仓信息')

    #撤单
    # xt_trader.cancel_order_stock(acc, 1082130954)

