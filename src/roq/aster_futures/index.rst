.. _roq-aster-futures:

.. |dagger| unicode:: U+2020
.. |double-dagger| unicode:: U+2021
.. |checkmark| unicode:: U+2713
.. |right-arrow| unicode:: U+2192
.. |right-double-arrow| unicode:: U+21D2
.. |left-right-double-arrow| unicode:: U+21D4


roq-aster-futures
=================


.. tab:: Unstable

  .. code-block:: shell

     $ conda install \
           --channel https://roq-trading.com/conda/unstable \
           roq-aster-futures

.. tab:: Stable

  .. code-block:: shell

     $ conda install \
           --channel https://roq-trading.com/conda/stable \
           roq-aster-futures


Supports
--------

.. grid::  2
  :gutter: 2

  .. grid-item-card::  Products

    .. list-table::
      :widths: auto
      :align: left

      * - :cpp:enumerator:`Spot <roq::SecurityType::SPOT>`
        -
        -
      * - :cpp:enumerator:`Futures <roq::SecurityType::FUTURES>`
        -
        -
      * - :cpp:enumerator:`Swap <roq::SecurityType::SWAP>`
        - |checkmark|
        -
      * - :cpp:enumerator:`Option <roq::SecurityType::OPTION>`
        -
        -

  .. grid-item-card::  Market Data

    .. list-table::
      :widths: auto
      :align: left

      * - :cpp:class:`ReferenceData <roq::ReferenceData>`
        - |checkmark|
        - |dagger|
      * - :cpp:class:`MarketStatus <roq::MarketStatus>`
        - |checkmark|
        - |dagger|
      * - :cpp:class:`TopOfBook <roq::TopOfBook>`
        - |checkmark|
        -
      * - :cpp:class:`MarketByPrice <roq::MarketByPriceUpdate>`
        - |checkmark|
        -
      * - :cpp:class:`MarketByOrder <roq::MarketByOrderUpdate>`
        -
        -
      * - :cpp:class:`TradeSummary <roq::TradeSummary>`
        - |checkmark|
        -
      * - :cpp:class:`Statistics <roq::StatisticsUpdate>`
        - |checkmark|
        -
      * - :cpp:class:`TimeSeries <roq::TimeSeriesUpdate>`
        -
        -

  .. grid-item-card::  Orders

    .. list-table::
      :widths: auto
      :align: left

      * - :cpp:class:`CreateOrder <roq::CreateOrder>`
        - |checkmark|
        -
      * - :cpp:class:`ModifyOrder <roq::ModifyOrder>`
        - |checkmark|
        -
      * - :cpp:class:`CancelOrder <roq::CancelOrder>`
        - |checkmark|
        -
      * - :cpp:class:`CancelAllOrders <roq::CancelAllOrders>`
        - |checkmark|
        -

  .. grid-item-card::  Account

    .. list-table::
      :widths: auto
      :align: left

      * - :cpp:class:`Funds <roq::FundsUpdate>`
        - |checkmark|
        -
      * - :cpp:class:`Position <roq::PositionUpdate>`
        - |checkmark|
        -

.. note::

   |dagger| The exchange protocol does not support streaming updates for reference data and market status.


Using
-----

.. code-block:: shell

   $ roq-aster-futures [FLAGS]


.. _roq-aster-futures-flags:

Flags
-----

.. code-block:: shell

   $ roq-aster-futures --help

.. tab:: Flags

   .. include:: flags/flags.rstinc

.. tab:: REST

   .. include:: flags/rest.rstinc

.. tab:: WS

   .. include:: flags/ws.rstinc

.. tab:: MBP

   .. include:: flags/mbp.rstinc

.. tab:: Misc

   .. include:: flags/misc.rstinc


Environments
------------

.. tab:: Prod

   .. code-block:: shell

      $ $CONDA_PREFIX/share/roq-aster-futures/flags/prod/flags.cfg

   .. include:: flags/prod/flags.cfg
     :code: shell


Configuration
-------------

.. code-block:: shell

   $ $CONDA_PREFIX/share/roq-aster-futures/config.toml

.. important::

   This template will be replaced when the software is upgraded.
   Make a copy and modify to your own needs.

.. include:: config.toml
   :code: toml


Market Data
-----------


Trading Status
~~~~~~~~~~~~~~

.. list-table::
  :header-rows: 1
  :widths: auto
  :align: left

  * - Enum
    -
    -

  * - :code:`TRADING`
    - |right-double-arrow|
    - :cpp:enumerator:`Open <roq::TradingStatus::OPEN>`

  * - :code:`PENDING_TRADING`
    - |right-double-arrow|
    - :cpp:enumerator:`PreOpen <roq::TradingStatus::PRE_OPEN>`

  * - :code:`SETTLING`
    - |right-double-arrow|
    - :cpp:enumerator:`Close <roq::TradingStatus::CLOSE>`


Statistics
~~~~~~~~~~

.. list-table::
  :header-rows: 1
  :widths: auto
  :align: left

  * - Event
    - Field
    - Comment
    -
    -

  * - :code:`MarkPriceUpdate`
    - :code:`p`
    - Mark price
    - |right-double-arrow|
    - :cpp:enumerator:`SettlementPrice <roq::StatisticsType::SETTLEMENT_PRICE>`

  * - :code:`MarkPriceUpdate`
    - :code:`P`
    - Est. settle price
    - |right-double-arrow|
    - :cpp:enumerator:`PreSettlementPrice <roq::StatisticsType::PRE_SETTLEMENT_PRICE>`

  * - :code:`MarkPriceUpdate`
    - :code:`i`
    - Index price
    - |right-double-arrow|
    - :cpp:enumerator:`IndexValue <roq::StatisticsType::INDEX_VALUE>`

  * - :code:`MarkPriceUpdate`
    - :code:`r`
    - Funding rate
    - |right-double-arrow|
    - :cpp:enumerator:`FundingRate <roq::StatisticsType::FUNDING_RATE>`

  * - :code:`MiniTicker`
    - :code:`o`
    - Open price
    - |right-double-arrow|
    - :cpp:enumerator:`OpenPrice <roq::StatisticsType::OPEN_PRICE>`

  * - :code:`MiniTicker`
    - :code:`h`
    - High price
    - |right-double-arrow|
    - :cpp:enumerator:`HighestTradedPrice <roq::StatisticsType::HIGHEST_TRADED_PRICE>`

  * - :code:`MiniTicker`
    - :code:`l`
    - Low price
    - |right-double-arrow|
    - :cpp:enumerator:`LowestTradedPrice <roq::StatisticsType::LOWEST_TRADED_PRICE>`

  * - :code:`MiniTicker`
    - :code:`c`
    - Close price
    - |right-double-arrow|
    - :cpp:enumerator:`ClosePrice <roq::StatisticsType::CLOSE_PRICE>`

  * - :code:`MiniTicker`
    - :code:`v`
    - Total volume (base)
    - |right-double-arrow|
    - :cpp:enumerator:`TradeVolume <roq::StatisticsType::TRADE_VOLUME>`


Orders
------


Position Effect
~~~~~~~~~~~~~~~

TBD


Margin Mode
~~~~~~~~~~~

TBD


Order Types
~~~~~~~~~~~

TBD


Time in Force
~~~~~~~~~~~~~

TBD


Execution Instructions
~~~~~~~~~~~~~~~~~~~~~~

TBD


Order Status
~~~~~~~~~~~~

TBD


Cancel-All
~~~~~~~~~~

TBD


Comments
--------

* The exchange supports auto-cancel on disconnect.
  This can be enabled using the flags.


References
----------

Common
~~~~~~

* :ref:`Using Conda <tutorial-conda>`
* :ref:`Using Flags <abseil-cpp>`
* :ref:`Gateway Flags <gateway-flags>`
* :ref:`Gateway Config <gateway-config>`

Exchange
~~~~~~~~

* `Website <https://www.asterdex.com/>`__
* `Documentation <https://docs.asterdex.com/>`__
* `Documentation <https://github.com/asterdex/api-docs/blob/master/aster-finance-futures-api-v3.md/>`__
