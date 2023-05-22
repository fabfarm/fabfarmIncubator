import React, { useEffect, useState } from "preact/compat";
import { useQuery } from "react-query";
import "./app.css";
import StatisticsCard from "./components/StatisticsCard";
import StatisticsChart from "./components/StatisticsChart";
import DropletIcon from "./components/icons/DropletIcon";
import TemperatureIcon from "./components/icons/TemperatureIcon";
import dailyHumidityChart from "./configs/humidity-config";
import dailyTemperatureChart from "./configs/temperature-config";
import { createStateTuple, prefetchData } from "./main";

type DataPoint = {
  x: number;
  y: number;
};

export const fetchDataWrapper = async (endpoint: string, payload?: any) =>
  await fetch(
    `http://${import.meta.env.VITE_SERVED_IP}/${endpoint}`,
    payload
      ? {
          method: "POST",
          body: JSON.stringify(payload),
          headers: { "Content-type": "application/json; charset=UTF-8" },
        }
      : undefined
  ).then((res) => res.json());

export const usePrefetchData = () =>
  useQuery({
    queryKey: ["prefetchData"],
    initialData: prefetchData,
    queryFn: () => fetchDataWrapper("fetchData"),
    refetchInterval: 10000,
  });

export const useCurrentTemperature = () =>
  useQuery({
    queryKey: ["getCurrentTemperature"],
    initialData: null,
    queryFn: () => fetchDataWrapper("getCurrentTemperature"),
    refetchInterval: 3000,
  });

export const useCurrentHumidity = () =>
  useQuery({
    queryKey: ["getCurrentHumidity"],
    initialData: null,
    queryFn: () => fetchDataWrapper("getCurrentHumidity"),
    refetchInterval: 2000,
  });

export const useTargetTemperature = () =>
  createStateTuple("getTargetTemperature", "setTargetTemperature");

export const useTargetHumidity = () =>
  createStateTuple("getTargetHumidity", "setTargetHumidity");

export const useTurnInterval = () =>
  createStateTuple("getInterval", "setInterval");
export const useTurnAngle = () => createStateTuple("getAngle", "setAngle");
export const useDebugMode = () =>
  createStateTuple("getDebugMode", "toggleDebugMode");
export const useOpStatus = () =>
  createStateTuple("getIncubatorStatus", "toggleIncubator");
export const usePID = () =>
  createStateTuple("getCurrentPidSettings", "updatePidSettings");

export function App() {
  let response = usePrefetchData().data;
  let currentHumidity = useCurrentHumidity().data?.humidity ?? "N/A";
  let currentTemperature = useCurrentTemperature().data?.temperature ?? "N/A";

  const [humidityXAxisData, setHumidityXAxisData] = useState([]);
  const [humidityYAxisData, setHumidityYAxisData] = useState([]);
  const [temperatureXAxisData, setTemperatureXAxisData] = useState([]);
  const [temperatureYAxisData, setTemperatureYAxisData] = useState([]);

  useEffect(() => {
    if (response) {
      const humidityResponse = response[0]?.[1];
      const temperatureResponse = response[0]?.[0];
      if (humidityResponse?.data) {
        const humidityData = humidityResponse.data.map((obj: DataPoint) => ({
          y: obj.y,
          x: new Date(obj.x).getDate(),
        }));
        setHumidityYAxisData(
          humidityData.map((obj: DataPoint) => obj.y?.toFixed(1))
        );
        setHumidityXAxisData(humidityData.map((obj: DataPoint) => obj.x));
      }

      if (temperatureResponse?.data) {
        const temperatureData = temperatureResponse.data.map(
          (obj: DataPoint) => ({
            y: obj.y,
            x: new Date(obj.x).getDate(),
          })
        );
        setTemperatureYAxisData(
          temperatureData.map((obj: DataPoint) => obj.y?.toFixed(1))
        );
        setTemperatureXAxisData(temperatureData.map((obj: DataPoint) => obj.x));
      }
    }
  }, [response]);
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
            value={currentHumidity}
            title="Current Rel. Humidity"
          ></StatisticsCard>
          <StatisticsCard
            color="red"
            icon={React.createElement(TemperatureIcon, {
              className: "h-6 w-6 text-white",
            })}
            value={currentTemperature}
            title="Current Temp"
          ></StatisticsCard>
        </div>
        <div className="mb-6 grid grid-cols-1 gap-y-12 gap-x-6 md:grid-cols-2 xl:grid-cols-3">
          <StatisticsChart
            color="blue"
            description="Rel. Humidity"
            chart={dailyHumidityChart}
            title="Daily Humidity"
            xAxisData={humidityXAxisData}
            yAxisData={humidityYAxisData}
          ></StatisticsChart>
          <StatisticsChart
            color="red"
            description="Temperature in Celcius"
            chart={dailyTemperatureChart}
            title="Daily Temperature"
            xAxisData={temperatureXAxisData}
            yAxisData={temperatureYAxisData}
          ></StatisticsChart>
        </div>
      </main>
    </>
  );
}
