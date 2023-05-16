import React from "preact/compat";
import "./app.css";
import StatisticsCard from "./components/StatisticsCard";
import StatisticsChart from "./components/StatisticsChart";
import DropletIcon from "./components/icons/DropletIcon";
import TemperatureIcon from "./components/icons/TemperatureIcon";
import dailyHumidityChart from "./configs/humidity-config";
import dailyTemperatureChart from "./configs/temperature-config";
import { useQuery } from "react-query";
import { Spinner } from "@material-tailwind/react";

export function App() {
  const {
    isLoading,
    error,
    data,
  }: { isLoading: boolean; error: any; data: any } = useQuery({
    queryKey: ["fetchData"],
    queryFn: () =>
      fetch([import.meta.env.VITE_SERVED_IP, "fetchData"].join("/"))
        .then((res) => res.json())
        .then((data) => console.log(data)),
  });

  if (isLoading) return <Spinner className="w-20 h-20 m-8"></Spinner>;

  if (error)
    return (
      <>
        {"An error has occurred, check that you are connecting to the correct IP: " +
          error.message}
      </>
    );

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
      </main>
    </>
  );
}
