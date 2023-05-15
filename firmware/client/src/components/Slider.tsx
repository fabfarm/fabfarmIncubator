import { Slider } from "@material-tailwind/react";

export default function SliderControl() {
  return (
    <div className="w-96">
      <Slider defaultValue={50} size="sm" />
    </div>
  );
}
