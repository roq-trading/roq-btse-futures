.. _roq-btse-futures:

.. |dagger| unicode:: U+2020
.. |double-dagger| unicode:: U+2021
.. |right-arrow| unicode:: U+2192
.. |right-double-arrow| unicode:: U+21D2
.. |left-right-double-arrow| unicode:: U+21D4
.. |check-mark| unicode:: U+2705
.. |cross-mark| unicode:: U+274C
.. |negative-cross-mark| unicode:: U+274E
.. |footnote-1| unicode:: U+2776
.. |footnote-2| unicode:: U+2777
.. |footnote-3| unicode:: U+2778


roq-btse-futures
================


.. tab:: Unstable

  .. code-block:: shell

     $ conda install \
           --channel https://roq-trading.com/conda/unstable \
           roq-btse-futures

.. tab:: Stable

  .. code-block:: shell

     $ conda install \
           --channel https://roq-trading.com/conda/stable \
           roq-btse-futures


Supports
--------

.. grid::  2
  :gutter: 2

  .. grid-item-card::  Products

    .. list-table::
      :widths: auto
      :align: left

      * - :cpp:enumerator:`Spot <roq::SecurityType::SPOT>`
        - |cross-mark|
        -
      * - :cpp:enumerator:`Futures <roq::SecurityType::FUTURES>`
        - |check-mark|
        -
      * - :cpp:enumerator:`Swap <roq::SecurityType::SWAP>`
        - |check-mark|
        -
      * - :cpp:enumerator:`Option <roq::SecurityType::OPTION>`
        - |cross-mark|
        -

  .. grid-item-card::  Market Data

    .. list-table::
      :widths: auto
      :align: left

      * - :cpp:class:`ReferenceData <roq::ReferenceData>`
        - |check-mark|
        - |footnote-1|
      * - :cpp:class:`MarketStatus <roq::MarketStatus>`
        - |check-mark|
        - |footnote-1|
      * - :cpp:class:`TopOfBook <roq::TopOfBook>`
        - |check-mark|
        -
      * - :cpp:class:`MarketByPrice <roq::MarketByPriceUpdate>`
        - |check-mark|
        -
      * - :cpp:class:`MarketByOrder <roq::MarketByOrderUpdate>`
        - |cross-mark|
        -
      * - :cpp:class:`TradeSummary <roq::TradeSummary>`
        - |check-mark|
        -
      * - :cpp:class:`Statistics <roq::StatisticsUpdate>`
        - |cross-mark|
        -
      * - :cpp:class:`TimeSeries <roq::TimeSeriesUpdate>`
        - |cross-mark|
        -

  .. grid-item-card::  Orders & Quotes

    .. list-table::
      :widths: auto
      :align: left

      * - :cpp:class:`CreateOrder <roq::CreateOrder>`
        - |negative-cross-mark|
        -
      * - :cpp:class:`ModifyOrder <roq::ModifyOrder>`
        - |negative-cross-mark|
        -
      * - :cpp:class:`CancelOrder <roq::CancelOrder>`
        - |negative-cross-mark|
        -
      * - :cpp:class:`CancelAllOrders <roq::CancelAllOrders>`
        - |cross-mark|
        -
      * - :cpp:class:`MassQuote <roq::MassQuote>`
        - |cross-mark|
        -
      * - :cpp:class:`CancelQuotes <roq::CancelQuotes>`
        - |cross-mark|
        -

  .. grid-item-card::  Account

    .. list-table::
      :widths: auto
      :align: left

      * - :cpp:class:`Funds <roq::FundsUpdate>`
        - |cross-mark|
        -
      * - :cpp:class:`Position <roq::PositionUpdate>`
        - |negative-cross-mark|
        -


.. note::

   |check-mark| = Available.

   |negative-cross-mark| = Not implemented.

   |cross-mark| = Unavailable.

   |footnote-1| = The exchange protocol does not support streaming updates for reference data and market status.


Using
-----

.. code-block:: shell

   $ roq-btse-futures [FLAGS]


.. _roq-btse-futures-flags:

Flags
-----

.. code-block:: shell

   $ roq-btse-futures --help

.. tab:: Flags

   .. include:: flags/flags.rstinc

.. tab:: REST

   .. include:: flags/rest.rstinc

.. tab:: WS

   .. include:: flags/ws.rstinc

.. tab:: Misc

   .. include:: flags/misc.rstinc


Environments
------------

.. tab:: Prod

   .. code-block:: shell

      $ --flagfile $CONDA_PREFIX/share/roq-btse-futures/flags/prod/flags.cfg

   .. include:: flags/prod/flags.cfg
     :code: shell

.. tab:: Test

   .. code-block:: shell

      $ --flagfile $CONDA_PREFIX/share/roq-btse-futures/flags/test/flags.cfg

   .. include:: flags/test/flags.cfg
     :code: shell


Configuration
-------------

.. code-block:: shell

   $ --flagfile $CONDA_PREFIX/share/roq-btse-futures/config.toml

.. important::

   The template will be replaced when the software is upgraded.
   Make a copy and modify to your needs.

.. include:: config.toml
   :code: toml


Market Data
-----------


Inbound
~~~~~~~

.. tab:: TradingStatus

   .. list-table::
     :header-rows: 1
     :widths: auto
     :align: left

     * - Event
       - Field
       - Comment
       -
       -

     * - :code:`MarketSummary`
       - :code:`active`
       - :code:`= false`
       - |right-double-arrow|
       - :cpp:enumerator:`HALT <roq::TradingStatus::HALT>`

     * - :code:`MarketSummary`
       - :code:`active`
       - :code:`= true`
       - |right-double-arrow|
       - :cpp:enumerator:`OPEN <roq::TradingStatus::OPEN>`


Order Management
----------------


Inbound
~~~~~~~


Outbound
~~~~~~~~


Comments
--------

* TODO check if order book max-depth is really 50 when applying incremental updates
* TODO order management hasn't been implemented


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

* `Website <https://www.btse.com/>`__
* `Documentation <https://btsecom.github.io/docs/futures/>`__
