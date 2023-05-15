import React from "preact/compat";
import "./app.css";
import SideNavDrawer from "./components/SideNavDrawer";
import StatisticsCard from "./components/StatisticsCard";
import StatisticsChart from "./components/StatisticsChart";
import chartsConfig from "./configs/chart-config";
import DropletIcon from "./components/icons/DropletIcon";
import TemperatureIcon from "./components/icons/TemperatureIcon";

const dailyHumidityChart = {
  type: "line",
  height: 220,
  series: [
    {
      name: "Humidity",
      data: [50, 40, 300, 320, 500, 350, 200, 230, 500],
    },
  ],
  options: {
    ...chartsConfig,
    colors: ["#fff"],
    stroke: {
      lineCap: "round",
    },
    markers: {
      size: 5,
    },
    xaxis: {
      ...chartsConfig.xaxis,
      categories: [
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec",
      ],
    },
  },
};

const dailyTemperatureChart = {
  type: "line",
  height: 220,
  series: [
    {
      name: "Temperature",
      data: [50, 40, 300, 320, 500, 350, 200, 230, 500],
    },
  ],
  options: {
    ...chartsConfig,
    colors: ["#fff"],
    stroke: {
      lineCap: "round",
    },
    markers: {
      size: 5,
    },
    xaxis: {
      ...chartsConfig.xaxis,
      categories: [
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec",
      ],
    },
  },
};

export function App() {
  return (
    <>
      <header>🥚 Incubator</header>
      <main className="m-8">
        <div className="mb-12 grid gap-y-10 gap-x-6 md:grid-cols-2 xl:grid-cols-4">
          <StatisticsCard
            color="blue"
            icon={React.createElement(DropletIcon, {
              className: "h-6 w-6 text-white",
            })}
            value={"50"}
            title="Current Rel. Humidity"
          ></StatisticsCard>
          <StatisticsCard
            color="red"
            icon={React.createElement(TemperatureIcon, {
              className: "h-6 w-6 text-white",
            })}
            value={"30°C"}
            title="Current Temp"
          ></StatisticsCard>
        </div>
        <div className="mb-6 grid grid-cols-1 gap-y-12 gap-x-6 md:grid-cols-2 xl:grid-cols-3">
          <StatisticsChart
            color="blue"
            description="Rel. Humidity"
            chart={dailyHumidityChart}
            title="Daily Humidity"
          ></StatisticsChart>
          <StatisticsChart
            color="red"
            description="Temperature in Celcius"
            chart={dailyTemperatureChart}
            title="Daily Temperature"
          ></StatisticsChart>
        </div>
        <div className="absolute bottom-8 right-8">
          <SideNavDrawer></SideNavDrawer>
        </div>
      </main>
    </>
  );
}
